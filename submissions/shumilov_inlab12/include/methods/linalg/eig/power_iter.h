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


enum class EigenValueUpdate: int
{
    PowerIteration = 0, RayleighQuotient = 1,
};


[[nodiscard]]
inline auto read_eig_update_algorithm(std::istream& in) -> EigenValueUpdate
{
    const auto algo = read_nonnegative_value<int>(in, "Eigenvalue Update Policy");
    if (algo > 3)
    {
        throw std::runtime_error(fmt::format("Invalid algorithm code, must be 0/1: {}", algo));
    }

    switch (algo)
    {
        case 0:
            return EigenValueUpdate::PowerIteration;
        case 1:
            return EigenValueUpdate::RayleighQuotient;
        default:
            throw std::runtime_error("Invalid algorithm code");
    }
}


template<>
struct fmt::formatter<EigenValueUpdate, char>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    constexpr auto format(const EigenValueUpdate val, format_context& ctx) const
    {
        switch (val)
        {
            case EigenValueUpdate::PowerIteration:
                return fmt::format_to(ctx.out(), "Power Iteration");
            case EigenValueUpdate::RayleighQuotient:
                return fmt::format_to(ctx.out(), "Rayleigh Quotient");
            default:
                std::unreachable();
        }
        return ctx.out();
    }
};


template<std::floating_point T>
struct PowerIterState final : FPState<T>
{
    // Main Operator
    std::shared_ptr<const Matrix<T>> matrix{};

    // Iteration Result
    std::vector<T> eigenvector{}; // eigenvector
    T eigenvalue{};               // Eigenvalue
    T eigenvalue_error{};

    EigenValueUpdate eigval_update{ EigenValueUpdate::PowerIteration };

    [[nodiscard]]
    constexpr explicit PowerIterState(
        std::shared_ptr<const Matrix<T>> A_,
        std::vector<T>&& guess,
        const EigenValueUpdate eval_update_ = EigenValueUpdate::PowerIteration
    ) : FPState<T>{}
      , matrix{ A_ }
      , eigenvector{ std::move(guess) }
      , eigval_update{ eval_update_ }
    {
        if (std::all_of(
            eigenvector.cbegin(),
            eigenvector.cend(),
            [&](const T& x) { return isclose<T>(x, T{}, 1e-8, 1e-12); }
        ))
        {
            throw std::invalid_argument("Eigenvector guess is too close to zero");
        }

        if (eigval_update == EigenValueUpdate::RayleighQuotient)
        {
            if (const auto idx = find_matrix_assymetry<T>(A(), T{}, 1e-12);
                idx.has_value())
            {

                const auto& [i, j] = idx.value();
                throw std::invalid_argument(
                    fmt::format("`A` is asymmetric in ({}, {}): {} != {}", i, j, A(i, j), A(j, i))
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
            if (eigval_update == EigenValueUpdate::RayleighQuotient)
            {
                const auto x_dot_x = dot<T>(x, x);
                return dot<T>(x, Ax) / x_dot_x;
            }

            const auto k = argmax<T>(Ax);
            return Ax[k] / x[k];
        }();

        this->eigenvalue_error = rel_diff(new_eval, this->eigenvalue);
        this->eigenvalue = new_eval;

        Ax /= norm_linf(Ax);
        this->m_error = max_rel_diff(Ax, x);

        std::swap(Ax, this->eigenvector);

        FPState<T>::update();
    }

    [[nodiscard]]
    constexpr auto residual() const
    {
        auto z = A() * x();
        return z - x() * this->eigenvalue;
    }

    private:
        [[nodiscard]]
        constexpr auto x() const -> const std::vector<T>&
        {
            return this->eigenvector;
        }

        [[nodiscard]]
        constexpr auto A() const -> const Matrix<T>&
        {
            return *(this->matrix);
        }

        [[nodiscard]]
        constexpr auto A(const int i, const int j) const -> const T&
        {
            return matrix->operator[](i, j);
        }

};


template<std::floating_point T>
struct PowerIteration : FixedPoint<T>
{
    EigenValueUpdate eval_update{ EigenValueUpdate::PowerIteration };

    [[nodiscard]]
    explicit constexpr PowerIteration
    (
        const FPSettings<T>& fps,
        const EigenValueUpdate eval_update_
    ) : FixedPoint<T>{ fps }
      , eval_update{ eval_update_ }
    {}

    [[nodiscard]]
    auto solve(std::shared_ptr<const Matrix<T>> A, std::vector<T>&& guess) const
    {
        assert(A->is_square());
        return FixedPoint<T>::template solve<PowerIterState<T>>(A, std::move(guess), eval_update);
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
        const auto residual = state->residual();
        return fmt::format_to(
            ctx.out(),
            "Converged: {}\n"
            "# Iterations: {:L}\n"
            "Max Abs Residual: {: 14.8e}\n"
            "Residual:\n{:14.8e}\n"
            "Eigenvalue Error: {: 14.8e}\n"
            "Eigenvalue: {:14.8e}\n"
            "Eigenvector Error: {:14.8e}\n"
            "Eigenvector:\n{: 14.8e}",
            converged,
            state->iteration(),
            max_abs(residual),
            fmt::join(residual, " "),
            state->eigenvalue_error,
            state->eigenvalue,
            state->error(),
            fmt::join(state->eigenvector, " ")
        );
    }
};


#endif //LINALG_EIG_POWER_ITER_H
