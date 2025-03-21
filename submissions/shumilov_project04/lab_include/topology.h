//
// Created by Kirill Shumilov on 3/20/25.
//

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <mpi.h>

#include "methods/utils/grid.h"
#include "mpi_utils.h"
#include "methods/stencil.h"


constexpr auto get_perfect_square(const int x)
{
    if (x >= 0)
    {
       if (const long double sr = sqrt(x);
          sr * sr == x)
          return std::make_optional(static_cast<int>(sr));
    }

    return std::optional<int>{std::nullopt};
}

struct CartesianTopology
{
   Indexer2D<> indexer{1, 1};
   int local_offset{};
   std::pair<int, int> local_idx{};
   std::map<Direction2D, int> neighbors{};

   [[nodiscard]]
   constexpr auto is_red() const
   {
      return (local_idx.first + local_idx.second) % 2 == 0;
   }

   [[nodiscard]]
   constexpr auto is_black() const
   {
      return (local_idx.first + local_idx.second) % 2 != 0;
   }

   [[nodiscard]]
   constexpr auto n_blocks_x() const
   {
      return indexer.rows();
   }

   [[nodiscard]]
   constexpr auto n_blocks_y() const
   {
      return indexer.rows();
   }

   [[nodiscard]]
   constexpr auto n_blocks() const
   {
      return n_blocks_x() * n_blocks_y();
   }

   [[nodiscard]]
   static auto from_mpi_comm(MPI_Comm comm)
   {
      const auto size = get_world_size(comm);
      const auto sr = get_perfect_square(size);
      if (not sr.has_value())
      {
         throw std::invalid_argument(
            fmt::format("The number of MPI Ranks must be a perfect square: {}", size)
         );
      }

      const Indexer2D indexer{sr.value(), sr.value()};
      const auto rank = get_local_rank(comm);

      return CartesianTopology{
         indexer,
         rank,
         indexer.unravel(rank),
         indexer.get_neighbors(rank)
      };
   }
};

#endif //TOPOLOGY_H
