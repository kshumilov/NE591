#ifndef CONJUGATE_GRADIENT_H
#define CONJUGATE_GRADIENT_H

#include <concepts>
#include <vector>
#include <memory>

#include <fmt/core.h>

#include "methods/fixed_point.h"
#include "methods/linalg/matrix.h"
#include "methods/linalg/utils/math.h"
#include "methods/linalg/Axb/utils.h"


struct CGParams
{
    int residual_update_frequency{10};

    [[nodiscard]]
    constexpr auto update_residual(const int iter) const
    {
        return iter % residual_update_frequency == 0;
    }
};


template<std::floating_point T>
struct CGState final : public FPState<T>
{
    std::shared_ptr<const LinearSystem<T>> system{};
    const CGParams params{};

    std::vector<T> x{};
    std::vector<T> r{};
    std::vector<T> d{};

    [[nodiscard]]
    constexpr CGState(
        std::shared_ptr<const LinearSystem<T>> Ab,
        const CGParams params_
    ) : FPState<T>{}
      , system{ Ab }
      , params{ params_ }
      , x(Ab->b.size(), 0)
      , r(Ab->b.cbegin(), Ab->b.cend())
      , d(Ab->b.cbegin(), Ab->b.cend())
    {
        CGState::validate_system(*system);
        this->m_error = norm_l2(r);
    }

    static auto validate_system(const LinearSystem<T>& system)
    {
        const auto& A = system.A;

        if (not A.is_square())
        {
            throw std::invalid_argument("`A` must be a square matrix: ()");
        }
        if (const auto idx = find_matrix_assymetry<T>(A, T{}, 1e-12);
            idx.has_value())
        {
            const auto& [i, j] = idx.value();
            throw std::invalid_argument(
                fmt::format("`A` is asymmetric in ({}, {}): {} != {}", i, j, A[i, j], A[j, i])
            );
        }
    }

    void update() override
    {
        const auto& A = system->A;
        const auto& b = system->b;

        // fmt::println(std::cerr, "r  : {}", r);
        // fmt::println(std::cerr, "d  : {}", d);
        // fmt::println(std::cerr, "x  : {}", x);

        const auto Ad = A * d;

        // fmt::println(std::cerr, "Ad : {}", r);

        const auto rprev_dot_rprev = this->m_error * this->m_error;
        const auto alpha = rprev_dot_rprev / dot(d, Ad);
        // fmt::println(std::cerr, "a  : {}", alpha);

        // Update the solution
        axpy<T>(d, x, alpha);
        // fmt::println(std::cerr, "x_n: {}", x);

        // Get new residual
        if (params.update_residual(this->iteration()))
        {
            std::copy(b.cbegin(), b.cend(), r.begin());
            gemv<T>(A, x, r, -T{ 1 }, T{ 1 });
        }
        else
        {
            axpy<T>(Ad, r, -alpha);
        }

        // fmt::println(std::cerr, "r_n: {}", r);

        // Get new Conjugate direction
        const auto r_dot_r = dot(r, r);
        const auto beta = r_dot_r / rprev_dot_rprev;
        scal<T>(d, beta);
        axpy<T>(r, d);

        this->m_error = std::sqrt(r_dot_r);

        FPState<T>::update();
    }
};


template<std::floating_point T>
struct fmt::formatter<CGState<T>>
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

    auto format(const CGState<T>& state, fmt::format_context& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "CG : ");
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

        if (style == Style::Full)
        {
            out = fmt::format_to(out, "\n");
            out = fmt_vec(state.x, "r");

            out = fmt::format_to(out, "\n");
            out = fmt_vec(state.d, "d");
        }
        return out;
    }
};


template<std::floating_point T, std::floating_point ErrorType = T>
class CG : public FixedPoint<ErrorType>
{
    CGParams params{};

    public:
        [[nodiscard]]
        explicit constexpr CG(
            const FPSettings<ErrorType>& fps,
            const CGParams params_ = {}
        ) : FixedPoint<ErrorType>{ fps }
          , params{ params_ }
        {}


        [[nodiscard]]
        auto solve(std::shared_ptr<LinearSystem<T>> system) const
        {
            return FixedPoint<ErrorType>::template solve<CGState<T>>(system, params);
        }
};

#endif //CONJUGATE_GRADIENT_H
