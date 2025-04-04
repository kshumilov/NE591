#ifndef SOR_H
#define SOR_H

#include <concepts>
#include <limits>

#include "array.h"
#include "block.h"
#include "domain.h"
#include "result.h"
#include "stencil.h"
#include "residual.h"

template<std::floating_point T>
auto sor
(
   Distributed2DBlock<T>&& x,
   const Stencil2D<T>& A,
   const Distributed2DBlock<T>& b,
   const T relaxation_factor,
   const FixedPointSettings<T>& settings,
   const MPIDomain2D& domain
)
{
   T error{ std::numeric_limits<T>::infinity() };

   const int col_i = x.info.halo.west;
   const int col_f = col_i + x.info.local.cols();

   const auto b_ = b.padded_array_view();
   const auto x_cv = x.padded_array_cview();
   auto x_mv = x.padded_array_view();

   int iter{ 0 };
   for (; iter < settings.max_iter; ++iter)
   {
      if (error < settings.tolerance)
         break;

      T local_error{ 0 };
      auto body = [&] (const int i, const int j)
      {
         const auto dx = relaxation_factor * (b_[i - 1, j - 1] - A.apply(i, j, x_cv)) / A.center;
         local_error = std::max(rel_err(dx, x_cv[i, j]), local_error);
         x_mv[i, j] += dx;
      };

      // Black Squares
      for (const auto i : x.iter_internal_rows())
         for (int j{ col_i + 1 - i % 2 }; j < col_f; j += 2)
            body(i, j);

      x.exchange_padding(domain);

      // Red Squares
      for (const auto i : x.iter_internal_rows())
         for (int j{ col_i + i % 2 }; j < col_f; j += 2)
            body(i, j);

      x.exchange_padding(domain);

      MPI_Allreduce(
         &local_error,
         &error,
         1,
         get_mpi_type<T>(),
         MPI_MAX,
         domain.cart_comm
      );
   }

   return FixedPointResult<T>{
      error < settings.tolerance,
      error,
      iter,
      max_abs_residual(A, b, x, domain),
      std::move(x)
   };
}
#endif //POINT_JACOBI_H
