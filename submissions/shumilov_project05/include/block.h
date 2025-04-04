#ifndef BLOCK_H
#define BLOCK_H

#include <ostream>
#include <concepts>
#include <map>
#include <vector>
#include <ranges>

#include <fmt/core.h>
#include <fmt/ostream.h>

#ifndef NDEBUG
#include <iostream>
#endif

#include "neighborhood.h"
#include "shape.h"
#include "domain.h"
#include "matrix.h"
#include "utils.h"


template<std::floating_point T>
struct Block2DInfo
{
    Shape2D global{};
    Shape2D local{};
    Padding halo{};

    // MPI Datatypes for efficient communication of ghost layers
    MPI_Datatype col_dt{ MPI_DATATYPE_NULL }; // For east/west exchange
    MPI_Datatype row_dt{ MPI_DATATYPE_NULL }; // For north/south exchange

    // MPI Datatypes for block scattering and aggregation
    MPI_Datatype aux_block_type_{ MPI_DATATYPE_NULL };
    MPI_Datatype global_block_dt{ MPI_DATATYPE_NULL };
    MPI_Datatype local_block_dt{ MPI_DATATYPE_NULL };

    // Function to determine local grid size based on global size and rank coords
    [[nodiscard]]
    Block2DInfo(const MPIDomain2D& domain, const Shape2D& shape, const Padding& padding = Padding{})
        : global{ shape }
        , local{
            global.rows() / domain.rows(),
            global.cols() / domain.cols(),
        }
        , halo{ padding }
    {
        if (global.rows() % domain.rows() != 0 || global.cols() % domain.cols() != 0) {
            MPI_Abort(domain.comm, 1);
//             if (domain.rank == 0) std::cerr << "Warning: Grid dimensions not perfectly divisible by process dimensions.\n";
        }

        create_mpi_datatypes();
    }

    Block2DInfo(const Block2DInfo& other) = delete;
    Block2DInfo& operator=(const Block2DInfo& other) = delete;

    Block2DInfo(Block2DInfo&& other) noexcept
        : global{ std::move(other.global) }
        , local{ std::move(other.local) }
        , halo{ std::move(other.halo) }
        , col_dt{ other.col_dt }
        , row_dt{ other.row_dt }
        , aux_block_type_{ other.aux_block_type_ }
        , global_block_dt{ other.global_block_dt }
        , local_block_dt{ other.local_block_dt }
    {
        other.col_dt = MPI_DATATYPE_NULL;

        other.row_dt = MPI_DATATYPE_NULL;
        other.aux_block_type_ = MPI_DATATYPE_NULL;
        other.global_block_dt = MPI_DATATYPE_NULL;
        other.local_block_dt = MPI_DATATYPE_NULL;
    }

    ~Block2DInfo() {
         if (local_block_dt != MPI_DATATYPE_NULL)
             MPI_Type_free(&local_block_dt);

         if (global_block_dt != MPI_DATATYPE_NULL)
             MPI_Type_free(&global_block_dt);

         if (aux_block_type_ != MPI_DATATYPE_NULL)
             MPI_Type_free(&aux_block_type_);

         if (row_dt != MPI_DATATYPE_NULL)
             MPI_Type_free(&row_dt);

         if (col_dt != MPI_DATATYPE_NULL)
             MPI_Type_free(&col_dt);
    }

    [[nodiscard]]
    constexpr auto padded_rows() const
    {
        return local.rows() + halo.north + halo.south;
    }

    [[nodiscard]]
    constexpr auto padded_cols() const
    {
        return local.cols() + halo.west + halo.east;
    }

    [[nodiscard]]
    constexpr auto padded_size() const
    {
        return padded_rows() * padded_cols();
    }

    [[nodiscard]]
    constexpr auto padded_shape() const
    {
        return local.padded(halo);
    }

    auto create_block_displacements(const MPIDomain2D& domain) const
    {
        std::vector<int> displacements{};
        for (const auto i : std::views::iota(0, domain.rows()))
        {
            const auto block_offset = i * local.rows() * domain.cols();
            for (const auto j : std::views::iota(0, domain.cols()))
                displacements.emplace_back(block_offset + j);
        }
        return displacements;
    }

    private:
        void create_mpi_datatypes()
        {
            // Create a vector type for columns (stride = padded width)
            // Accessing element (row j, col i) in flux_view_(j, i)
            // A column is elements (1, i), (2, i), ..., (N_internal, i)
            MPI_Type_vector(
                local.rows() /* column length */,
                1 /* column width = 1 by def */,
                padded_cols() /* stride between column elements in row major order, in local padded block */,
                get_mpi_type<T>(),
                &col_dt
            );
            MPI_Type_commit(&col_dt);

            // Create a contiguous type for rows (since they are contiguous in memory)
            // A row is elements (j, 1), (j, 2), ..., (j, N_internal)
            MPI_Type_contiguous(local.cols(), get_mpi_type<T>(), &row_dt);
            MPI_Type_commit(&row_dt);


            // Scatter Block Type
            MPI_Type_vector(
                local.rows() /* Number rows in a block */,
                local.cols() /* Elements per row */,
                global.cols() /* Stride between first elements of consecutive rows */,
                get_mpi_type<T>(),
                &aux_block_type_
            );
            MPI_Type_commit(&aux_block_type_);

            MPI_Type_create_resized(
                aux_block_type_,
                0,
                local.cols() * sizeof(T),
                &global_block_dt
            );
            MPI_Type_commit(&global_block_dt);

            MPI_Type_vector(
                local.rows(),
                local.cols(),
                padded_cols(),
                get_mpi_type<T>(),
                &local_block_dt
            );
            MPI_Type_commit(&local_block_dt);
        }
};


template<std::floating_point T>
struct Distributed2DBlock {
    Block2DInfo<T> info;
    std::vector<T> data{};

    [[nodiscard]]
    Distributed2DBlock(
        Block2DInfo<T>&& info_,
        std::vector<T>&& data_
    )
        : info{ std::move(info_) }
        , data{ std::move(data_) }
    {}

    [[nodiscard]]
    Distributed2DBlock(
        const MPIDomain2D& domain,
        const Shape2D& global_shape,
        const Padding& padding = Padding{}
    )
        : info{ domain, global_shape, padding }
        , data(info.padded_size(), T{})
    {}

    [[nodiscard]]
    Distributed2DBlock(
        const MPIDomain2D& domain,
        const Shape2D& global_shape,
        std::vector<T>&& data_,
        const Padding& padding = Padding{}
    )
        : info{ domain, global_shape, padding }
        , data{ std::move(data_) }
    {
        // TODO: use sync state
        // if (not block_info.padded_points() == data.size())
        // {
        //     MPI_Abort(domain.cart_comm, EXIT_FAILURE);
        // }
    }

    [[nodiscard]]
    Distributed2DBlock(Distributed2DBlock&& other) noexcept
        : info{ std::move(other.info) }
        , data{ std::move(other.data) }
    {}

    // Delete copy operations
    Distributed2DBlock(const Distributed2DBlock&) = delete;
    Distributed2DBlock& operator=(const Distributed2DBlock&) = delete;
    Distributed2DBlock& operator=(Distributed2DBlock&&) = delete;

    [[nodiscard]]
    static auto zeros_like(
        const MPIDomain2D& domain,
        const Distributed2DBlock& other,
        const Padding& padding = Padding{}
    )
    {
        return Distributed2DBlock{
            domain,
            other.info.global,
            padding,
        };
    }

    [[nodiscard]]
    static auto scatter(
        const MPIDomain2D& domain,
        const Shape2D& global_shape,
        const std::vector<T>& global_data,
        const int root = 0,
        const Padding& padding = Padding{}
    )
    {
        // if (not static_cast<std::size_t>(global_shape.nelems()) != global_data.size())
        //     throw std::invalid_argument("Shape Mismatch");

        Block2DInfo<T> info{ domain, global_shape, padding };
        std::vector<T> local_data(info.padded_size());

        MatrixView<T> local_view{local_data, info.padded_shape()};

        const std::vector count(domain.size, 1);
        const auto block_displacements = info.create_block_displacements(domain);

        #ifndef NDEBUG
        if (domain.is_manager())
        {
            fmt::println(std::cerr, "Block Displacements:\n{}", block_displacements);
        }
        #endif

        MPI_Scatterv(
            global_data.data(),
            count.data(),
            block_displacements.data(),
            info.global_block_dt,
            &local_view[info.halo.north, info.halo.west],
            1,
            info.local_block_dt,
            root,
            domain.cart_comm
        );

        return Distributed2DBlock{ std::move(info), std::move(local_data) };
    }

    [[maybe_unused]]
    auto gather(const MPIDomain2D& domain, const int root = 0)
    {
        std::vector<T> collected_data = [&]
        {
            if (domain.rank == root)
                return std::vector<T>(info.global.nelems());

            return std::vector<T>{};
        }();

        const std::vector count(domain.size, 1);
        const auto block_displacements = info.create_block_displacements(domain);

        auto view = padded_array_view();

        const auto result = MPI_Gatherv(
            &view[info.halo.north, info.halo.west],
            1, info.local_block_dt,
            collected_data.data(),
            count.data(),
            block_displacements.data(),
            info.global_block_dt,
            root,
            domain.cart_comm
        );

        if (domain.rank == root)
        {
            std::swap(collected_data, data);
            info.local = info.global;
            info.halo = Padding{};
        }

        return result;
    }

    // Exchange ghost cell data with neighbors using non-blocking MPI
    void exchange_padding(const MPIDomain2D& domain) {
        MPI_Request reqs[8]; // Max 4 sends, 4 receives
        int req_count = 0;

        auto view = padded_array_view();
        const auto& [N, S, E, W] = info.halo;  // Padding
        const auto R = info.padded_rows(); // Total number of rows in block, R = r + N + S
        const auto C = info.padded_cols(); // Total number of cols in block, C = c + W + E

        // Send to North (sending local northern boundary to other's southern bondary)
        MPI_Isend(
            &view[N, W], N, info.row_dt,
            domain.neighbors.north,0, domain.cart_comm,
            &reqs[req_count++]
        );

        // Recv from South
        MPI_Irecv(
            &view[R - S, W], S, info.row_dt,
            domain.neighbors.south, 0, domain.cart_comm,
            &reqs[req_count++]
        );

        // Send to South (send north boundary)
        // Start of S internal row
        MPI_Isend(
            &view[R - 2 * S, W], S, info.row_dt,
            domain.neighbors.south, 1, domain.cart_comm,
            &reqs[req_count++]
        );

        // Recv from North
        // Start of N internal rows
        MPI_Irecv(
            &view[0, W], N, info.row_dt,
            domain.neighbors.north, 1, domain.cart_comm,
            &reqs[req_count++]
        );

        // Send to East
        MPI_Isend(
            &view[N, C - 2 * E], E, info.col_dt,
            domain.neighbors.east, 2, domain.cart_comm,
            &reqs[req_count++]
        );

        // Recv from West
        MPI_Irecv(
            &view[N, 0], W, info.col_dt,
            domain.neighbors.west, 2, domain.cart_comm,
            &reqs[req_count++]
        );

        // Send to West
        MPI_Isend(
            &view[N, W], W, info.col_dt,
            domain.neighbors.west, 3, domain.cart_comm,
            &reqs[req_count++]
        );

        // Recv from East
        MPI_Irecv(
            &view[N, C - E], E, info.col_dt,
            domain.neighbors.east, 3, domain.cart_comm,
            &reqs[req_count++]
        );

        // Wait for all non-blocking communications to complete
        if (req_count > 0) {
            MPI_Waitall(req_count, reqs, MPI_STATUSES_IGNORE);
        }
    }

    [[nodiscard]]
    constexpr auto size() const
    {
        return data.size();
    }

    [[nodiscard]]
    constexpr auto iter_rows() const
    {
       return std::views::iota(0, info.padded_rows());
    }

    [[nodiscard]]
    constexpr auto iter_cols() const
    {
       return std::views::iota(0, info.padded_cols());
    }

    [[nodiscard]]
    constexpr auto iter_internal_rows() const
    {
       return std::views::iota(
           info.halo.north,
           info.halo.north + info.local.rows()
       );
    }

    [[nodiscard]]
    constexpr auto iter_internal_cols() const
    {
       return std::views::iota(
           info.halo.east,
           info.halo.east + info.local.cols()
       );
    }

    [[nodiscard]]
    constexpr auto padded_array_cview() const
    {
        return MatrixView<const T>{ data, info.padded_shape()};
    }

    [[nodiscard]]
    constexpr auto padded_array_view() const
    {
        return MatrixView<const T>{ data, info.padded_shape()};
    }

    [[nodiscard]]
    constexpr auto padded_array_view()
    {
        return MatrixView<T>{ data, info.padded_shape()};
    }

    auto display(std::ostream& out, const MPIDomain2D& domain) const
    {
        for (int i = 0; i < domain.size; i++)
        {
            MPI_Barrier(domain.cart_comm);
            display(out, domain, i);
        }
    }

    auto display(std::ostream& out, const MPIDomain2D& domain, const int rank) const
    {
        if (rank == domain.rank)
        {
            const auto view = padded_array_view();
            fmt::println(
                out,
                "Rank {}, <{}, {}>:\n{}\n",
                domain.rank,
                info.padded_rows(),
                info.padded_cols(),
                view.to_string()
            );
        }
    }
};


#endif //BLOCK_H
