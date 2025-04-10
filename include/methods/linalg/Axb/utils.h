#ifndef LINALG_AXB_UTILS_H
#define LINALG_AXB_UTILS_H

#include <concepts>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "methods/fixed_point.h"

#include "methods/linalg/matrix.h"
#include "methods/utils/io.h"
#include "methods/linalg/utils/io.h"

#include "methods/linalg/Axb/algorithm.h"

template<std::floating_point T>
struct LinearSystem
{
    Matrix<T> A{};
    std::vector<T> b{};

    [[nodiscard]]
    constexpr LinearSystem(Matrix<T>&& A_, std::vector<T>&& b_)
        : A{ std::move(A_) }
        , b{ std::move(b_) }
    {
        if (not A.is_square())
        {
            throw std::invalid_argument("`A` must be a square matrix: ()");
        }

        if (not LinearSystem::matches_shape(A, b))
        {
            throw std::invalid_argument(
                fmt::format("Shape mismatch: ({}, {}) & ({})", A.rows(), A.cols(), b.size())
            );
        }
    }

    [[nodiscard]]
    static auto matches_shape(const Matrix<T>& A, std::vector<T>& b)
    {
        return A.rows() == b.size();
    }

    [[nodiscard]]
    constexpr auto rank() const
    {
        return static_cast<int>(A.rows());
    }

    [[nodiscard]]
    constexpr auto residual(const std::vector<T>& x) const
    {
        return get_residual<T>(A, x, b);
    }

    [[nodiscard]]
    static auto from_file(std::istream& input) -> LinearSystem
    {
        const auto rank = static_cast<std::size_t>(read_positive_value<int>(input, "Matrix rank n"));
        return LinearSystem{
            read_square_matrix<T, MatrixSymmetry::General>(input, rank),
            read_vector<T>(input, rank)
        };
    }
};


template<std::floating_point T>
struct fmt::formatter<LinearSystem<T>>
{

    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const LinearSystem<T>& system, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "Matrix, A: {:F: 14.8e}\n\n"
            "RHS Vector, b:\n[{: 14.8e}]",
            system.A, fmt::join(system.b, " ")
        );
    }
};


template<std::floating_point T>
struct IterAxbState : FPState<T>
{
    std::shared_ptr<const LinearSystem<T>> system{};

    std::vector<T> x{};

    [[nodiscard]]
    explicit constexpr IterAxbState(std::shared_ptr<const LinearSystem<T>> Ab)
        : system{ Ab }
        , x(Ab->A.cols(), T{})
    {}

    virtual auto update() -> void
    {
        this->m_iter += 1;
    }

    [[nodiscard]]
    constexpr auto residual() const
    {
        return system->residual(x);
    }

    [[nodiscard]]
    virtual AxbAlgorithm algorithm() const
    {
        std::unreachable();
    }
};


template<std::floating_point T>
struct fmt::formatter<IterAxbState<T>>
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

    auto format(const IterAxbState<T>& state, fmt::format_context& ctx) const
    {
        auto out = ctx.out();
        ctx.advance_to(out);
        out = underlying.format(state, ctx);

        if (style == Style::Solution or style == Style::Full)
        {
            out = fmt::format_to(out, ":\n");
            ctx.advance_to(out);
            out = format_vec(state.x, "x", ctx);
        }
        return out;
    }

    auto format_vec(const std::vector<T>& data, const std::string_view label, fmt::format_context& ctx) const
    {
        auto out = ctx.out();
        out = fmt::format_to(out, "{}: [", label);
        bool first = true;
        for (const auto& v : data)
        {
            if (not first)
                out = fmt::format_to(out, " ");

            ctx.advance_to(out);
            underlying.real_fmt.format(v, ctx);
            first = false;
        }
        return fmt::format_to(out, "]");
    }
};


template<std::floating_point DType>
struct IterativeAxbResult
{
    std::vector<DType> x{};
    DType relative_error{};
    DType residual_error{};
    bool converged{};
    int iters{};


    [[nodiscard]]
    constexpr auto to_string() const -> std::string {
        if (converged) {
            return fmt::format("Converged at iteration #{: <5d}:\n"
                               "\tRelative error: {:12.6e}\n"
                               "\tResidual error: {:12.6e}",
                               iters, relative_error, residual_error);
        }
        return fmt::format("Failed to converge in  #{: <5d}:\n"
                               "\tRelative error: {:12.6e}\n"
                               "\tResidual error: {:12.6e}",
                               iters, relative_error, residual_error);
    }
};

#endif //LINALG_AXB_UTILS_H
