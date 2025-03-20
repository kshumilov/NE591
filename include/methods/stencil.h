//
// Created by Kirill Shumilov on 3/20/25.
//

#ifndef STENCIL_H
#define STENCIL_H

#include "methods/linalg/matrix.h"
#include "methods/utils/grid.h"


template<std::floating_point T>
struct ConstantStencil2D
{
    Indexer2D<> shape{3, 3};
    T m_top{}, m_bottom{}, m_left{}, m_right{}, m_center{ 1 };

    template<ApplyOrdering ordering = ApplyOrdering::Sequential>
    constexpr auto apply(std::invocable<int, int> auto func) const -> void
    {
        shape.apply_inner<ordering>(func);
    }


    [[nodiscard]]
    constexpr auto operator()(const int i, const int j, const Matrix<T>& u) const
    {
        assert(is_valid_matrix(u));
        assert(shape.is_valid_inner_idx(i, j));
        return peripheral(i, j, u) + center(i, j) * u[i, j];
    }


    [[nodiscard]]
    constexpr auto center(const int, const int) const
    {
        return m_center;
    }


    [[nodiscard]]
    constexpr auto peripheral(const int i, const int j, const Matrix<T>& u) const
    {
        assert(is_valid_matrix(u));
        assert(shape.is_valid_inner_idx(i, j));
        return (
            m_bottom * u[i - 1, j] +
            m_top    * u[i + 1, j] +
            m_left   * u[i, j - 1] +
            m_right  * u[i, j + 1]
        );
    }

    [[nodiscard]]
    constexpr auto max_residual(const Matrix<T>& u, const Matrix<T>& f) const
    {
        assert(is_valid_matrix(u));
        assert(u.rows() == f.rows() + 2 && u.cols() == f.cols() + 2);

        T max_residual{};
        shape.apply_inner<ApplyOrdering::Sequential>(
            [&](const int i, const int j)
            {
                if (const auto residual = std::abs(f[i - 1, j - 1] - this->operator()(i, j, u));
                    residual > max_residual)
                {
                    max_residual = residual;
                }
            }
        );
        return max_residual;
    }

    [[nodiscard]]
    constexpr auto build_matrix() const
    {
        const auto inner_shape = this->shape.get_inner_indexer();
        return Matrix<T>::from_func(
            inner_shape.nelems(),
            [&](const auto I, const auto J)
            {
                if (I == J)
                    return this->m_center;

                const auto [i_f, j_f] = inner_shape.unravel(static_cast<int>(I));
                const auto [i_u, j_u] = inner_shape.unravel(static_cast<int>(J));

                if (j_u == j_f)
                {
                    if (0 < i_f && i_f - 1 == i_u)
                        return m_bottom;

                    if (i_f + 1 < inner_shape.rows() && i_f + 1 == i_u)
                        return m_top;
                }
                else if (i_u == i_f)
                {
                    if (0 < j_f && j_f - 1 == j_u)
                        return m_left;

                    if (j_f + 1 < inner_shape.cols() && j_f + 1 == j_u)
                        return m_right;
                }

                return T{};
            }
        );
    }


    [[nodiscard]]
    constexpr auto is_valid_matrix(const Matrix<T>& m) const -> bool
    {
        return (m.cols() == static_cast<std::size_t>(shape.cols()) &&
                m.rows() == static_cast<std::size_t>(shape.rows()));
    }
};
#endif //STENCIL_H
