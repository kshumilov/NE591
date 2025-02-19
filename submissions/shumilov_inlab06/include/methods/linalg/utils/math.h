#ifndef LINALG_MATH_H
#define LINALG_MATH_H

#include <ranges>
#include <vector>
#include <cstdlib>

#include "methods/linalg/matrix.h"


template<class T>
[[nodiscard]] constexpr
Matrix<T> extract_lowerunit_inplace(Matrix<T>& A) {
    auto L = Matrix<T>::eye(A.rows(), A.cols());

    for (const auto i : A.iter_rows()) {
        for (const auto j : std::views::iota(0U, i)) {
            std::swap(L[i, j], A[i, j]);
        }
    }

    return L;
}


template<class T>
[[nodiscard]] constexpr
auto extract_diagonal_inplace(Matrix<T>& A) -> std::vector<T> {
    std::vector<T> D(A.rows(), T{});
    for (std::size_t i = 0; i < A.rows(); ++i) {
        std::swap(D[i], A[i, i]);
    }

    return D;
}


/**
 * @brief Calculates residual r = b - A * x, for linear system Ax = b
 *
 * @tparam T Scalar real type
 * @param A Matrix A
 * @param x Candidate solution
 * @param b RHS vector
 *
 * @return residual vector
 */
template<class T>
[[nodiscard]] constexpr
auto get_residual(const Matrix<T>& A, std::span<const T> x, std::span<const T> b) -> std::vector<T> {
    assert(A.cols() == x.size());
    assert(A.rows() == b.size());

    std::vector<T> residual{ b.cbegin(), b.cend() };
    gemv<T>(A, x, residual, T{ -1 });
    return residual;
}


#endif //LINALG_MATH_H
