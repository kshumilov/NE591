#ifndef POINT_JACOBI_H
#define POINT_JACOBI_H

#include <concepts>
#include <limits>
#include <vector>

#include "array.h"
#include "block.h"
#include "domain.h"
#include "result.h"
#include "stencil.h"
#include "residual.h"


template<std::floating_point T>
auto point_jacobi
(
   Distributed2DBlock<T>&& x,
   const Stencil2D<T>& A,
   const Distributed2DBlock<T>& b,
   const FixedPointSettings<T>& settings,
   const MPIDomain2D& domain
)
{
   T error{ std::numeric_limits<T>::infinity() };

   std::vector<T> dx(b.size());

   MatrixView<T> dx_{ dx, b.info.local };
   const auto b_ = b.padded_array_view();
   const auto x_cv = x.padded_array_cview();
   auto x_mv = x.padded_array_view();

   int iter{ 0 };
   for (; iter < settings.max_iter; ++iter)
   {
      if (error < settings.tolerance)
         break;

      #ifndef NDEBUG
      if (domain.is_manager())
      {
         fmt::println(std::cerr, "Iter #{: 5d}/{: 5d}: {:14.8e}", iter, settings.max_iter, error);
         fmt::println(std::cerr, "x:");
      }
      x.display(std::cerr, domain);
      #endif

      T local_error{ 0 };
      for (const auto i : b.iter_rows())
         for (const auto j : b.iter_cols())
         {
            dx_[i, j] = (b_[i, j] - A.apply(i + 1, j + 1, x_cv)) / A.center;
            local_error = std::max(rel_err(dx_[i, j], x_cv[i + 1, j + 1]), local_error);
         }

      for (const auto i : b.iter_rows())
         for (const auto j : b.iter_cols())
            x_mv[i + 1, j + 1] += dx_[i, j];

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
