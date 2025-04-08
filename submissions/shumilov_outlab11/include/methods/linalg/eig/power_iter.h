#ifndef LINALG_EIG_POWER_ITER_H
#define LINALG_EIG_POWER_ITER_H

#include <concepts>
#include <memory>

#include <fmt/core.h>
#include <fmt/ranges.h>


#include "methods/linalg/utils/math.h"
#include "methods/array.h"

#include "methods/fixed_point.h"
#include "methods/linalg/matrix.h"


enum class PowerIterationAlgo: int
{
    PowerIteration = 0,
    RayleighQuotient = 1,
};


template<std::floating_point T>
struct PowerIterState final : FPState<T> {
    using Result = std::pair<bool, std::unique_ptr<PowerIterState<T>>>;

    // Main Operator
    std::shared_ptr<const Matrix<T>> matrix{};

    // Iteration Result
    std::vector<T> eigenvector{}; // eigenvector
    T eigenvalue{}; // Eigenvalue
    T eigenvalue_error{};

    PowerIterationAlgo algo{PowerIterationAlgo::PowerIteration};

    [[nodiscard]]
    constexpr explicit PowerIterState(
        std::shared_ptr<const Matrix<T>> A,
        std::vector<T>&& guess,
        const PowerIterationAlgo algo_
    ) : FPState<T>{}
      , matrix{ A }
      , eigenvector{ std::move(guess) }
      , algo{ algo_ }
    {
        if (std::all_of(
            eigenvector.cbegin(),
            eigenvector.cend(),
            [&](const T& x) { return isclose<T>(x, T{}, 1e-8, 1e-12); }
        ))
        {
            throw std::invalid_argument("Eigen vector guess is too close to zero");
        }

        if (algo == PowerIterationAlgo::RayleighQuotient)
        {
            if (const auto idx = find_matrix_assymetry<T>(A, T{}, 1e-12);
                idx.has_value())
            {
                const auto& [i, j] = idx.value();
                throw std::invalid_argument(
                    fmt::format("`A` is asymmetric in ({}, {}): {} != {}", i, j, A()[i, j], A()[j, i])
                );
            }
        }
    }


    void update() override
    {
        const auto& A = this->A();
        const auto& x = this->x();

        auto Ax = A * x;

        const auto new_eval = [&]
        {
            if (algo == PowerIterationAlgo::RayleighQuotient)
            {
                const auto x_dot_x = dot<T>(x, x);
                return dot<T>(x, Ax) / x_dot_x;
            }

            const auto k = argmax<T>(Ax);
            return Ax[k] / x[k];
        }();

        this->eigenvalue_error = rel_diff(new_eval, this->eigenvalue_error);
        this->eigenvalue = new_eval;

        Ax /= norm_linf(Ax);
        this->m_error = max_rel_diff(Ax, x);

        std::swap(Ax, this->eigenvector);

        FPState<T>::update();
    }

    [[nodiscard]]
    constexpr auto x() const -> const std::vector<T>&
    {
       return this->eigenvector;
    }

    [[nodiscard]]
    constexpr auto A() const -> const Matrix<T>&
    {
        return *this->matrix;
    }

    [[nodiscard]]
    constexpr auto max_abs_residual() const
    {
        auto z = A() * x();
        return max_abs(z - x() * this->eigenvalue);
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
    auto solve(std::shared_ptr<const Matrix<T>> A, std::vector<T>&& guess) const
    {
        assert(A->is_square());
        return FixedPoint<T>::template solve<PowerIterState<T>>(A, std::move(guess));
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
struct fmt::formatter<typename PowerIterState<T>::Result>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    auto format(const typename PowerIterState<T>::Result& result, format_context& ctx) const
    {
        const auto& [converged, state] = result;

        return fmt::format_to(ctx.out(),
            "Converged: {}\n"
            "# Iterations: {:L}\n"
            "Residual: {: 14.8e}"
            "Eigenvalue Error: {: 14.8e}\n"
            "Eigenvalue: {:14.8e}\n"
            "Eigenvector Error: {:14.8e}\n"
            "Eigenvector:\n"
            "{: 14.8e}",
            converged,
            state->iteration(),
            state->max_abs_residual(),
            state->eigenvalue_error,
            state->eigenvalue,
            state->error(),
            fmt::join(state->eigenvector, " ")
        );
    }
};


#endif //LINALG_EIG_POWER_ITER_H
