#ifndef LINALG_AXB_SOR_H
#define LINALG_AXB_SOR_H

#include <concepts>
#include <span>
#include <vector>
#include <memory>

#include "methods/array.h"
#include "methods/optimize.h"
#include "methods/linalg/matrix.h"
#include "methods/linalg/utils/math.h"

#include "methods/fixed_point.h"
#include "methods/linalg/Axb/utils.h"


struct SORParams
{
    int relaxation_factor{1};

    [[nodiscard]]
    explicit constexpr SORParams(const int relaxation_factor_ = 1) : relaxation_factor(relaxation_factor_)
    {
        if (relaxation_factor_ < 0)
        {
            throw std::invalid_argument(
                fmt::format("Relaxation factor must be non-negative: {}", relaxation_factor)
            );
        }
    }
};


template<std::floating_point T, std::floating_point ErrorType = T>
struct SORState final : public FPState<ErrorType>
{
    std::shared_ptr<const LinearSystem<T>> system{};
    const SORParams params{};

    std::vector<T> x{};

    [[nodiscard]]
    constexpr SORState(
        std::shared_ptr<const LinearSystem<T>> Ab,
        const SORParams params_
    ) : FPState<ErrorType>{}
      , system{ Ab }
      , params{ params_ }
      , x(Ab->b.size(), 0)
    {
        SORState::validate_system(*system);
    }

    void update() override
    {
        const auto& A = system->A;
        const auto& b = system->b;
        const auto& w = this->params.relaxation_factor;

        this->m_error = T{};
        for (const auto i : A.iter_rows())
        {
            const T update = w * (b[i] - dot(A.row(i), x)) / A[i, i];
            this->m_error = std::max(rel_err(update, x[i]), this->m_error);
            x[i] += update;
        }

        FPState<ErrorType>::update();
    }

    static auto validate_system(const LinearSystem<T>& system)
    {
        const auto& A = system.A;

        if (not A.is_square())
        {
            throw std::invalid_argument(
                fmt::format("`A` must be a square matrix: {:r}", A)
            );
        }

        if (const auto idx = find_nonzero_diag(A); idx.has_value())
        {
            const int& i = idx.value();
            throw std::invalid_argument(
                fmt::format("`A` must have non-zero diagonal: A[{0}, {0}] = {1}", i, A[i, i])
            );
        }
    }
};


template<std::floating_point T>
struct fmt::formatter<SORState<T>>
{
    enum Style
    {
        Repr,
        Solution,
        Full,
    };

    Style style = Style::Repr;

    fmt::formatter<FPState<T>> underlying{};

    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        auto reached_end = [&](const auto& pos) -> bool
        {
            return pos == ctx.end() or *pos == '}';
        };

        auto it = ctx.begin();

        if (reached_end(it))
            return it;

        this->style = [&] {
            switch (*it++)
            {
                case 'r':
                    return Style::Repr;
                case 's':
                    return Style::Solution;
                case 'F':
                    return Style::Full;
                default:
                    throw std::format_error("Invalid style");
            }
        }();

        if (reached_end(it))
            return it;

        if (*it == ':')
        {
            ++it;
            ctx.advance_to(it);
            it = underlying.parse(ctx);
        }

        return it;
    }

    auto format(const SORState<T>& state, fmt::format_context& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "SOR: ");
        ctx.advance_to(out);
        out = underlying.format(state, ctx);

        auto fmt_vec = [&](const std::vector<T>& data, const std::string_view label)
        {
            out = fmt::format_to(out, "{}: [", label);
            bool first = true;
            for (const auto& v : data)
            {
                if (not first)
                {
                    out = fmt::format_to(out, " ");
                }
                ctx.advance_to(out);
                underlying.real_fmt.format(v, ctx);
                first = false;
            }
            return fmt::format_to(out, "]");
        };

        if (style == Style::Solution or style == Style::Full)
        {
            out = fmt::format_to(out, ":\n");
            out = fmt_vec(state.x, "x");
        }
        return out;
    }
};


template<std::floating_point T, std::floating_point ErrorType = T>
class SOR : public FixedPoint<ErrorType>
{
    SORParams params{};

    public:
        [[nodiscard]]
        explicit constexpr SOR(
            const FPSettings<ErrorType>& fps,
            const SORParams params_ = SORParams{}
        ) : FixedPoint<ErrorType>{ fps }
          , params{ params_ }
        {}


        [[nodiscard]]
        auto solve(std::shared_ptr<LinearSystem<T>> system) const
        {
            return FixedPoint<ErrorType>::template solve<SORState<T>>(system, params);
        }
};

template<std::floating_point DType, std::invocable<std::size_t, std::size_t> MatElem>
constexpr auto successive_over_relaxation(
        MatElem A,
        std::span<const DType> b,
        const DType relaxation_factor,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
    assert(relaxation_factor >= 1.0);

    std::vector<DType> x(b.size());
    std::vector<DType> x_next(b.size());

    auto g = [&](std::span<DType> x_curr) constexpr -> std::span<DType>
    {
        for (std::size_t i{}; i < x_curr.size(); ++i)
        {
            DType dot_prod{};

            // j in [0, i)
            for (std::size_t j{}; j < i; ++j)
                dot_prod += A(i, j) * x_next[j];

            // skip j == i

            // j in [i + 1, A.cols())
            for (std::size_t j{ i + 1 }; j < x_curr.size(); ++j)
                dot_prod += A(i, j) * x_curr[j];

            x_next[i] = (1 - relaxation_factor) * x_curr[i]
                        + relaxation_factor * (b[i] - dot_prod) / A(i, i);
        }

        std::swap(x, x_next);

        return std::span{x};
    };

    const auto iter_result = fixed_point_iteration<std::span<DType>>(
       g, x, max_rel_diff<std::span<const DType>, std::span<const DType>>, settings
    );

    const auto residual = get_residual<DType>(A, x, b);

    return IterativeAxbResult<DType>{
        .x = std::move(x),
        .relative_error = iter_result.error,
        .residual_error = max_abs(residual),
        .converged = iter_result.converged,
        .iters = iter_result.iters
    };
}


template<std::floating_point DType>
constexpr auto successive_over_relaxation(
        const Matrix<DType>& A,
        std::span<const DType> b,
        const DType relaxation_factor,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
    assert(not A.empty());
    assert(A.is_square());
    assert(A.rows() == b.size());
    assert(is_diag_nonzero(A));

    auto matelem = [&A](const std::size_t i, const std::size_t j) -> DType
    {
        return A[i, j];
    };

    return successive_over_relaxation<DType>(matelem, b, relaxation_factor, settings);
}


template<std::floating_point DType>
constexpr auto successive_over_relaxation(
        const std::pair<Matrix<DType>, std::vector<DType>>& linear_system,
        const DType relaxation_factor,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType> {
    return successive_over_relaxation<DType>(linear_system.first, linear_system.second, relaxation_factor, settings);
}

#endif // LINALG_AXB_SOR_H