#ifndef STENCIL_H
#define STENCIL_H

#include <concepts>

#include "matrix.h"


template<std::floating_point T>
struct Stencil2D
{
    T center{};
    Neighborhood2D<T> neighborhood{};

    [[nodiscard]]
    constexpr auto apply_peripheral(const int i, const int j, const MatrixView<const T>& mat) const -> T
    {
        return (
            neighborhood.north * mat[i - 1, j] +
            neighborhood.south * mat[i + 1, j] +
            neighborhood.west  * mat[i, j - 1] +
            neighborhood.east  * mat[i, j + 1]
        );
    }

    [[nodiscard]]
    constexpr auto apply_center(const int i, const int j, const MatrixView<const T>& mat) const -> T
    {
        return center * mat[i, j];
    }

    [[nodiscard]]
    constexpr auto apply(const int i, const int j, const MatrixView<const T>& mat) const -> T
    {
        return apply_peripheral(i, j, mat) + apply_center(i, j, mat);
    }
};

#endif //STENCIL_H
