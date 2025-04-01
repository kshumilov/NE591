#ifndef LINALG_EIG_POWER_ITER_H
#define LINALG_EIG_POWER_ITER_H

#include <concepts>
#include <memory>

#include <fmt/core.h>
#include <fmt/ranges.h>


#include "methods/array.h"

#include "methods/fixed_point.h"
#include "methods/linalg/matrix.h"


template<std::floating_point T>
struct PowerIterState final : FPState<T> {
    std::shared_ptr<const Matrix<T>> matrix{};

    std::vector<T> eigenvector{};
    T eigenvalue{};

    [[nodiscard]]
    constexpr explicit PowerIterState(
        std::shared_ptr<const Matrix<T>> matrix_
    ) : FPState<T>{}
      , matrix{ matrix_ }
      , eigenvector(matrix_->cols(), T{})
    {
        eigenvector.at(0) = T{ 1 };
    }

    void update() override
    {
        const auto& A = *(this->matrix);

        auto Ax = A * this->eigenvector;
        const auto x_dot_x = dot<T>(this->eigenvector, this->eigenvector);
        this->eigenvalue = dot<T>(this->eigenvector, Ax) / x_dot_x;

        Ax /= max_abs(Ax);

        this->m_error = max_rel_diff(Ax, this->eigenvector);

        std::swap(Ax, this->eigenvector);

        FPState<T>::update();
    }
};


template<std::floating_point T>
struct PowerIteration : FixedPoint<T>
{
    [[nodiscard]]
    explicit constexpr PowerIteration(
        const FPSettings<T>& fps
    ) : FixedPoint<T>{ fps }
    {}

    [[nodiscard]]
    auto solve(std::shared_ptr<const Matrix<T>> A) const
    {
        assert(A->is_square());
        return FixedPoint<T>::template solve<PowerIterState<T>>(A);
    }
};


template<std::floating_point T>
struct fmt::formatter<PowerIterState<T>>
{
    formatter<FPState<T>> underlying{};

    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return underlying.parse(ctx);
    }

    auto format(const PowerIterState<T>& state, format_context& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "PI:");
        ctx.advance_to(out);
        return underlying.format(state, ctx);
    }
};


template<std::floating_point T>
using PowerIterResult = std::pair<bool, std::unique_ptr<PowerIterState<T>>>;


template<std::floating_point T>
struct fmt::formatter<PowerIterResult<T>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    auto format(const PowerIterResult<T>& result, format_context& ctx) const
    {
        const auto& [converged, state] = result;

        return fmt::format_to(ctx.out(),
            "Converged: {}\n"
            "# Iterations: {:L}\n"
            "Error: {:14.8e}\n"
            "Eigenvalue: {:14.8e}\n"
            "Eigenvector:\n"
            "{: 14.8e}",
            converged,
            state->iteration(),
            state->error(),
            state->eigenvalue,
            fmt::join(state->eigenvector, " ")
        );
    }
};


#endif //LINALG_EIG_POWER_ITER_H
