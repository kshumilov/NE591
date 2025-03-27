#ifndef CONJUGATE_GRADIENT_H
#define CONJUGATE_GRADIENT_H

#include <concepts>
#include <vector>

#include <fmt/core.h>

#include "methods/fixed_point.h"
#include "methods/linalg/matrix.h"
#include "methods/linalg/utils/math.h"


template<std::floating_point T, std::floating_point ErrorType = T>
struct CGState final : public FixedPoint<ErrorType>
{
    const Matrix<T>& A{};
    const std::vector<T>& b{};

    std::vector<T> d{}; // Current direction
    std::vector<T> x{}; //
    std::vector<T> r{};


    [[nodiscard]]
    CGState(
        const FPSettings<ErrorType>& fps,
        const Matrix<T>& A_,
        const std::vector<T>& b_
    ) : FixedPoint<ErrorType>{ fps }
      , A{ A_ }
      , b{ b_ }
      , d(b_.cbegin(), b_.cend())
      , x(b_.size(), 0)
      , r(b_.cbegin(), b_.cend())
    {
        CGState::validate_A(A);

        if (not matches_shape(A, b))
        {
            throw std::invalid_argument(
                fmt::format("Shape mismatch: ({}, {}) & ({})", A.rows(), A.cols(), b.size())
            );
        }

        this->m_error = norm_l2(r);
    }


    static auto validate_A(const Matrix<T>& A)
    {
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
        fmt::println(std::cerr, "r  : {}", r);
        fmt::println(std::cerr, "d  : {}", d);
        fmt::println(std::cerr, "x  : {}", x);

        const auto Ad = A * d;

        fmt::println(std::cerr, "Ad : {}", r);

        const auto r_dot_r = dot(r, r);
        this->m_error = std::sqrt(r_dot_r);

        const auto alpha = r_dot_r / dot(d, Ad);
        fmt::println(std::cerr, "a  : {}", alpha);

        axpy<T>(d, x, alpha);
        fmt::println(std::cerr, "x_n: {}", x);

        if (false)
        {
            std::copy(b.cbegin(), b.cend(), r.begin());
            gemv<T>(A, x, r, -T{ 1 }, T{ 1 });
        }
        else
        {
            axpy<T>(Ad, r, -alpha);
        }
        fmt::println(std::cerr, "r_n: {}", r);

        const auto beta = dot(r, r) / r_dot_r;
        scal<T>(d, beta);
        axpy<T>(r, d);

        FixedPoint<ErrorType>::update();
    }
};

#endif //CONJUGATE_GRADIENT_H
