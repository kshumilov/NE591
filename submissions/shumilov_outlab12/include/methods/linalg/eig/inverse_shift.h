#ifndef LINALG_EIG_INVERSE_SHIFT_H
#define LINALG_EIG_INVERSE_SHIFT_H

#include <concepts>
#include <memory>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include "methods/array.h"
#include "methods/linalg/utils/math.h"

#include "methods/fixed_point.h"
#include "methods/linalg/matrix.h"
#include "methods/linalg/eig/eig_update.h"


#include "methods/linalg/lu.h"


template<std::floating_point T>
auto solve(Matrix<T>&& A, std::span<const T> b) -> std::vector<T>
{
    auto LU = std::move(A);
    const auto [P, result] = lup_factor_inplace<T>(LU);
    return lup_solve<T>(LU, P, b);
}


template<std::floating_point T>
struct InverseShiftState final : FPState<T> {
    // Main Operator
    std::shared_ptr<const Matrix<T>> matrix{};

    // Iteration Result
    std::vector<T> eigenvector{}; // eigenvector
    T eigenvalue{};               // Eigenvalue

    T eigval_error{ std::numeric_limits<T>::infinity() };
    T eigvec_error{ std::numeric_limits<T>::infinity() };

    EigenValueUpdate eigval_update{ EigenValueUpdate::PowerIteration };
    T shift{};

    Matrix<T> I{};

    [[nodiscard]]
    constexpr explicit InverseShiftState
    (
        std::shared_ptr<const Matrix<T>> matrix_,
        std::vector<T>&& guess,
        const T shift_,
        const EigenValueUpdate eigval_update_
    ) : FPState<T>{}
      , matrix{ matrix_ }
      , eigenvector{ std::move(guess) }
      , eigenvalue{ shift_ }
      , eigval_update{ eigval_update_ }
      , shift{ shift_ }
      , I{ Matrix<T>::eye(eigenvector.size()) }
    {
        if (std::all_of(
            eigenvector.cbegin(),
            eigenvector.cend(),
            [&](const T& x) { return isclose<T>(x, T{}, 1e-8, 1e-12); }
        ))
        {
            throw std::invalid_argument("Eigenvector guess is too close to zero");
        }

        if (const auto idx = find_matrix_assymetry<T>(*matrix, T{}, 1e-12);
            idx.has_value())
        {
            const auto& [i, j] = idx.value();
            throw std::invalid_argument(
                fmt::format("`A` is asymmetric in ({}, {}): {} != {}", i, j, A(i, j), A(j, i))
            );
        }
    }

    void update() override
    {
        const auto& A = *(this->matrix);
        const auto& v = this->eigenvector;

        auto w = solve<T>(A - shift * I, v);

        const auto new_eigval = [&]
        {
            if (eigval_update == EigenValueUpdate::RayleighQuotient)
            {
                w /= norm_l2(w);
                const auto Aw = A * w;
                return dot(w, Aw);
            }
            else
            {
                w /= norm_linf(w);
                const auto Aw = A * w;
                const auto k = argmax<T>(Aw);
                return Aw[k] / w[k];
            }
        }();

        eigval_error = rel_diff<T, T>(new_eigval, eigenvalue);
        eigenvalue = new_eigval;

        w /= norm_linf(w);
        eigvec_error = max_rel_diff(w, v);
        std::swap(w, this->eigenvector);

        this->m_error = std::max(eigval_error, eigvec_error);
        FPState<T>::update();
    }

    [[nodiscard]]
    constexpr auto residual() const
    {
        auto z = *matrix * eigenvector;
        return z - eigenvector * eigenvalue;
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
            return *this->matrix;
        }

        [[nodiscard]]
        constexpr auto A(const int i, const int j) const -> const T&
        {
            return matrix->operator[](i, j);
        }
};


template<std::floating_point T>
struct InverseShift : FixedPoint<T>
{
    EigenValueUpdate eigval_update{ EigenValueUpdate::PowerIteration };

    [[nodiscard]]
    explicit constexpr InverseShift(
        const FPSettings<T>& fps,
        const EigenValueUpdate eigval_update_
    ) : FixedPoint<T>{ fps }
      , eigval_update{ eigval_update_ }
    {}

    [[nodiscard]]
    auto solve(
        std::shared_ptr<const Matrix<T>> A,
        std::vector<T>&& guess,
        const T shift
    ) const
    {
        // assert(A->is_square());
        return FixedPoint<T>::template solve<InverseShiftState<T>>(
            A,
            std::move(guess),
            shift,
            eigval_update
        );
    }
};


template<std::floating_point T>
struct fmt::formatter<InverseShiftState<T>>
{
    formatter<FPState<T>> underlying{};

    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return underlying.parse(ctx);
    }

    auto format(const InverseShiftState<T>& state, format_context& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "PI:");
        ctx.advance_to(out);
        return underlying.format(state, ctx);
    }
};


template<std::floating_point T>
using InverseShiftResult = std::pair<bool, std::unique_ptr<InverseShiftState<T>>>;


template<std::floating_point T>
struct fmt::formatter<InverseShiftResult<T>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    auto format(const InverseShiftResult<T>& result, format_context& ctx) const
    {
        const auto& [converged, state] = result;
        const auto residual = state->residual();
        return fmt::format_to(
            ctx.out(),
            "Converged: {}\n"
            "# Iterations: {:L}\n"
            "Iterative Error: {:14.8e}\n"
            "Max Abs Residual: {:14.8e}\n"
            "Residual:\n{: 14.8e}\n"
            "Eigenvalue Iter Error: {:14.8e}\n"
            "Eigenvalue: {:14.8e}\n"
            "Eigenvector Iter Error: {:14.8e}\n"
            "Eigenvector:\n{: 14.8e}",
            converged,
            state->iteration(),
            state->error(),
            max_abs(residual),
            fmt::join(residual, " "),
            state->eigval_error,
            state->eigenvalue,
            state->eigvec_error,
            fmt::join(state->eigenvector, " ")
        );
    }
};


#endif //LINALG_EIG_POWER_ITER_H
