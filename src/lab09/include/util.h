#ifndef LAB09_UTIL_H
#define LAB09_UTIL_H

#include <concepts>
#include <utility>
#include <optional>

#include "methods/linalg/matrix.h"
#include "methods/utils/math.h"

using Index2D = std::pair<std::size_t, std::size_t>;

template<std::floating_point T>
[[nodiscard]]
constexpr auto find_matrix_assymetry(const Matrix<T>& M, const T rtol = T{1.0e-05}, const T atol = T{1.0e-08}) -> std::optional<Index2D>
{
   for (const auto i : M.iter_rows())
   {
      for (const auto j : std::views::iota(i + 1U, M.cols()))
      {
         if (not isclose(M[i, j], M[j, i], rtol, atol))
         {
            return std::make_optional(Index2D{i, j});
         }
      }
   }
   return std::nullopt;
}


#endif //LAB09_UTIL_H
