#ifndef LINALG_EIG_INVERSE_SHIFT_H
#define LINALG_EIG_INVERSE_SHIFT_H

#include <concepts>
#include <memory>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include "methods/array.h"

#include "methods/fixed_point.h"
#include "methods/linalg/matrix.h"

#include "methods/linalg/lu.h"


template<std::floating_point T>
struct InverseShiftState final : FPState<T> {
    std::shared_ptr<const Matrix<T>> matrix{};

    std::vector<T> eigenvector{};
    T eigenvalue{};
    const Matrix<T> I{};

    [[nodiscard]]
    constexpr explicit InverseShiftState(
        std::shared_ptr<const Matrix<T>> matrix_,
        const std::vector<T>& eigenvector_,
        const T eigenvalue_
    ) : FPState<T>{}
      , matrix{ matrix_ }
      , eigenvector{eigenvector_}
      , eigenvalue{eigenvalue_}
      , I{ Matrix<T>::eye(eigenvector.size()) }
    {}

    void update() override
    {
        const auto& A = *(this->matrix);

        auto B = A - eigenvalue * I;
        const auto [P, result] = lup_factor_inplace(B);
        auto new_evec = lup_solve(B, P, eigenvector);
        new_evec /= norm_linf(new_evec);

        auto Ax = A * new_evec;
        this->eigenvalue = dot<T>(new_evec, Ax) / dot<T>(new_evec, new_evec);

        this->m_error = max_rel_diff(new_evec, this->eigenvector);

        std::swap(new_evec, this->eigenvector);

        FPState<T>::update();
    }
};


template<std::floating_point T>
struct InverseShift : FixedPoint<T>
{
    [[nodiscard]]
    explicit constexpr InverseShift(
        const FPSettings<T>& fps
    ) : FixedPoint<T>{ fps }
    {}

    [[nodiscard]]
    auto solve(std::shared_ptr<const Matrix<T>> A, const std::vector<T>& evec, const T eval) const
    {
        assert(A->is_square());
        return FixedPoint<T>::template solve<InverseShiftState<T>>(A, evec, eval);
    }
};


//template<std::floating_point T>
//struct fmt::formatter<PowerIerState<T>>
//{
//    formatter<FPState<T>> underlying{};
//
//    [[nodiscard]]
//    constexpr auto parse(format_parse_context& ctx)
//    {
//        return underlying.parse(ctx);
//    }
//
//    auto format(const PowerIterState<T>& state, format_context& ctx) const
//    {
//        auto out = fmt::format_to(ctx.out(), "PI:");
//        ctx.advance_to(out);
//        return underlying.format(state, ctx);
//    }
//};
//
//
//template<std::floating_point T>
//using PowerIterResult = std::pair<bool, std::unique_ptr<PowerIterState<T>>>;
//
//
//template<std::floating_point T>
//struct fmt::formatter<PowerIterResult<T>>
//{
//    [[nodiscard]]
//    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
//
//    auto format(const PowerIterResult<T>& result, format_context& ctx) const
//    {
//        const auto& [converged, state] = result;
//
//        return fmt::format_to(ctx.out(),
//            "Converged: {}\n"
//            "# Iterations: {:L}\n"
//            "Error: {:14.8e}\n"
//            "Eigenvalue: {:14.8e}\n"
//            "Eigenvector:\n"
//            "{: 14.8e}",
//            converged,
//            state->iteration(),
//            state->error(),
//            state->eigenvalue,
//            fmt::join(state->eigenvector, " ")
//        );
//    }
//};
//

#endif //LINALG_EIG_POWER_ITER_H
