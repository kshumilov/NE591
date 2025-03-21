#ifndef FINITE_DIFFERENCE_H
#define FINITE_DIFFERENCE_H

#include <complex>
#include <concepts>
#include <stdexcept>
#include <utility>
#include <ostream>

#include "methods/array.h"
#include "methods/optimize.h"
#include "methods/linalg/matrix.h"

#include "methods/utils/grid.h"
#include "methods/utils/math.h"
#include "methods/stencil.h"


template<std::floating_point T>
struct FiniteDifferenceResult
{
    Matrix<T> u{};
    bool converged{ false };
    int iters{};
    T iter_error{ std::numeric_limits<T>::infinity() };
    T max_abs_residual{ std::numeric_limits<T>::infinity() };

    [[nodiscard]]
    constexpr auto to_string() const -> std::string
    {
        return fmt::format(
            "{:} at #{:d} with error: {:14.8e}, max|res|: {:14.8e}",
            converged ? "SUCCESS" : "FAILURE",
            iters, iter_error, max_abs_residual
        );
    }

    [[nodiscard]]
    constexpr auto echo(std::ostream& out, const std::string& name = "flux") const
    {
        fmt::println(out, "{}", to_string());
        fmt::println(out, "{:^5s} {:^5s} {:^14s}", "i", "j", name);
        for (const auto i : u.iter_rows())
        {
            for (const auto j : u.iter_cols())
            {
                fmt::println(out, "{:5d} {:5d} {: 14.8e}", i + 1, j + 1, u[i, j]);
            }
        }
    }
};

template<std::floating_point T>
struct PointJacobiAlgorithm
{
    struct State
    {
        Matrix<T> curr{};
        Matrix<T> next{};


        [[nodiscard]]
        explicit constexpr State(const ConstantStencil2D<T>& stencil)
            : curr{ Matrix<T>::zeros(stencil.shape.rows(), stencil.shape.cols()) }
            , next{ Matrix<T>::zeros(stencil.shape.rows(), stencil.shape.cols()) }
        {}


        [[nodiscard]]
        constexpr auto rel_error() const -> T
        {
            return max_rel_diff(next.data(), curr.data());
        }

        constexpr auto swap_curr_next() noexcept -> void
        {
            curr.swap(next);
        }
    };

    [[nodiscard]]
    constexpr auto init(const ConstantStencil2D<T>& stencil, const Matrix<T>&) const
    {
        return State{ stencil };
    }


    [[nodiscard]]
    constexpr auto iter(State& u, const ConstantStencil2D<T>& stencil, const Matrix<T>& f) const
    {
        T max_rel_error{};
        stencil.apply(
            [&](const auto i, const auto j) -> void
            {
                const auto update = (f[i - 1, j - 1] - stencil(i, j, u.curr)) / stencil.center(i, j);
                u.next[i, j] = u.curr[i, j] + update;

                if (const auto error = rel_err(update, u.curr[i, j]);
                    error > max_rel_error)
                    max_rel_error = error;
            }
        );

        u.swap_curr_next();
        return max_rel_error;
    }

    [[nodiscard]]
    constexpr auto finalize(FixedPointIterResult<State, T>&& result, const ConstantStencil2D<T>& stencil, const Matrix<T>& f) const
    {
        const auto max_abs_error = stencil.max_residual(result.x.curr, f);
        return FiniteDifferenceResult<T>{
            .u = std::move(result.x.curr.submatrix(1, 1, stencil.shape.inner_rows(), stencil.shape.inner_cols())),
            .converged = result.converged,
            .iters = result.iters,
            .iter_error = result.error,
            .max_abs_residual= max_abs_error
        };
    }
};


template<std::floating_point T>
struct SuccessiveOverRelaxationAlgorithm
{
    T factor{1};

    using State = Matrix<T>;

    [[nodiscard]]
    constexpr auto init(const ConstantStencil2D<T>& stencil, const Matrix<T>& f) const
    {
        return State::zeros(stencil.shape.rows(), stencil.shape.cols());
    }

    [[nodiscard]]
    constexpr auto iter(State& u, const ConstantStencil2D<T>& stencil, const Matrix<T>& f) const
    {
        T max_rel_error{};
        stencil.template apply<ApplyOrdering::CheckerBoard>([&] (const auto i, const auto j)
        {
            const auto update = factor * (f[i - 1, j - 1] - stencil(i, j, u)) / stencil.center(i, j);

            if (const auto error = rel_err(update, u[i, j]); error > max_rel_error)
                max_rel_error = error;

            u[i, j] += update;
        });

        return max_rel_error;
    }

    [[nodiscard]]
    constexpr auto finalize(FixedPointIterResult<State, T>&& result, const ConstantStencil2D<T>& stencil, const Matrix<T>& f) const
    {
        const auto max_residual = stencil.max_residual(result.x, f);
        return FiniteDifferenceResult<T>{
            .u = std::move(result.x),
            .converged = result.converged,
            .iters = result.iters,
            .iter_error = result.error,
            .residual_norm = max_residual
        };
    }
};


template<std::floating_point T>
struct GaussSeidelAlgorithm : SuccessiveOverRelaxationAlgorithm<T>
{
    explicit constexpr GaussSeidelAlgorithm() : SuccessiveOverRelaxationAlgorithm<T>() {}
};


template<std::floating_point T, class Algorithm>
struct FiniteDifference {
    Algorithm algorithm{};
    FixedPointIterSettings<T> iter_settings{};

    [[nodiscard]]
    constexpr auto solve(const ConstantStencil2D<T>& stencil, const Matrix<T>& f) const
    {
        auto result = fixed_point_iteration(
            [&](typename Algorithm::State& state) constexpr
            {
                return algorithm.iter(state, stencil, f);
            },
            algorithm.init(stencil, f),
            iter_settings
        );
        return algorithm.finalize(std::move(result), stencil, f);
    }

};


template<std::floating_point T, class Algorithm>
constexpr auto make_finite_difference(Algorithm algo, const FixedPointIterSettings<T>& iter_settings)
{
    return FiniteDifference<T, Algorithm>{algo, iter_settings};
}
#endif //FINITE_DIFFERENCE_H
