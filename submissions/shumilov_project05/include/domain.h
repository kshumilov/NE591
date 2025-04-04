//
// Created by Kirill Shumilov on 4/2/25.
//

#ifndef DOMAIN_H
#define DOMAIN_H

#include <utility>

#include <mpi.h>

#include <fmt/format.h>

#ifndef NDEBUG
#include <iostream>
#include <fmt/ostream.h>
#endif

#include "neighborhood.h"


struct MPIDomain2D {
    static constexpr int NDIMS{ 2 };

    enum class Direction : int
    {
        Vertical = 0,
        Horizontal = 1,
    };

    int manager{};
    int rank{};
    int size{};
    MPI_Comm comm = MPI_COMM_WORLD;

    // Decomposition info (e.g., Cartesian communicator)
    MPI_Comm cart_comm{ MPI_COMM_NULL };
    int dims[NDIMS] = {0, 0}; // Process grid dimensions
    int coords[NDIMS] = {0, 0};        // Coordinates of this rank in process grid
    int periods[NDIMS] = {0, 0}; // Non-periodic boundaries

    // Neighbor ranks
    Neighborhood2D<int> neighbors{};

    [[nodiscard]]
    MPIDomain2D(const int manager_ = 0) {
        manager = manager_;
        MPI_Comm_rank(comm, &rank);
        MPI_Comm_size(comm, &size);

        // Create a 2D Cartesian topology
        MPI_Dims_create(size, NDIMS, dims);
        MPI_Cart_create(comm, NDIMS, dims, periods, 1 /*reorder*/, &cart_comm);
        MPI_Cart_coords(cart_comm, rank, NDIMS, coords);

        // x-direction neighbors
        MPI_Cart_shift(
            cart_comm,
            std::to_underlying(Direction::Vertical), 1,
            &neighbors.north, &neighbors.south
        );

        // y-direction neighbors
        MPI_Cart_shift(
            cart_comm,
            std::to_underlying(Direction::Horizontal), 1,
            &neighbors.west, &neighbors.east
        );
    }

    ~MPIDomain2D() {
        if (cart_comm != MPI_COMM_NULL && cart_comm != MPI_COMM_WORLD) {
            MPI_Comm_free(&cart_comm);
        }
    }

    // Delete copy/move operations for simplicity or implement correctly
    MPIDomain2D(const MPIDomain2D&) = delete;
    MPIDomain2D& operator=(const MPIDomain2D&) = delete;
    MPIDomain2D(MPIDomain2D&&) = delete;
    MPIDomain2D& operator=(MPIDomain2D&&) = delete;

    [[nodiscard]]
    constexpr auto rows() const
    {
        return dims[std::to_underlying(Direction::Vertical)];
    }

    [[nodiscard]]
    constexpr auto cols() const
    {
        return dims[std::to_underlying(Direction::Horizontal)];
    }

    [[nodiscard]]
    constexpr auto is_manager() const
    {
        return rank == manager;
    }
};


template<>
struct fmt::formatter<MPIDomain2D>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const MPIDomain2D& domain, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "{2:^{1}s}\n"
            "{0:-^{1}s}\n"
            "Processes: {3}\n"
            "Topology: Cartesian\n"
            "Dimensions: {4: 5d} x {5: 5d}\n"
            "{0:=^{1}s}",
            "", 80,
            "MPI Parameters",
            domain.size, domain.rows(), domain.cols()
        );
    }
};

#endif //DOMAIN_H
