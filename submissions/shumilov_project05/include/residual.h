#ifndef RESIDUAL_H
#define RESIDUAL_H

#include <concepts>

#include "stencil.h"
#include "cmath"
#include "block.h"


template<std::floating_point T>
[[nodiscard]]
auto max_abs_residual(
    const Stencil2D<T>& A,
    const Distributed2DBlock<T>& b,
    const Distributed2DBlock<T>& x,
    const MPIDomain2D& domain
)
{
    const auto b_ = b.padded_array_view();
    const auto x_cv = x.padded_array_cview();

    T local{};
    for (const auto i : b.iter_rows())
        for (const auto j : b.iter_cols())
        {
            const T residual = (b_[i, j] - A.apply(i + 1, j + 1, x_cv));
            local = std::max(std::abs(residual), local);
        }

    T global{};
    MPI_Allreduce(
        &local,
        &global,
        1,
        get_mpi_type<T>(),
        MPI_MAX,
        domain.cart_comm
    );

    return global;
}

#endif //RESIDUAL_H
