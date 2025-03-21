#ifndef MPI_UTILS_H
#define MPI_UTILS_H

#include <mpi.h>
#include <concepts>
#include <type_traits>

#include "methods/utils/grid.h"
#include "methods/optimize.h"
#include "methods/stencil.h"


auto get_local_rank(MPI_Comm comm) -> int;
auto get_world_size(MPI_Comm comm) -> int;
auto get_processor_name() -> std::string;

auto create_mpi_indexer2d_type() -> MPI_Datatype;


// Template function to map C++ floating-point types to MPI floating-point types
template <std::floating_point T>
[[nodiscard]]
constexpr MPI_Datatype get_mpi_type() {
    if constexpr (std::is_same_v<T, float>) {
        return MPI_FLOAT;
    } else if constexpr (std::is_same_v<T, double>) {
        return MPI_DOUBLE;
    } else if constexpr (std::is_same_v<T, long double>) {
        return MPI_LONG_DOUBLE;
    } else {
        static_assert(sizeof(T) == 0, "Unsupported floating-point type");
    }
}


template<std::floating_point T>
[[nodiscard]]
auto create_mpi_iter_settings_type() -> MPI_Datatype {
    constexpr int n_fields = 2;
    constexpr int block_lengths[n_fields] = {1, 1};
    const MPI_Datatype types[n_fields] = {
        get_mpi_type<T>(),
        MPI_INT,
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(FixedPointIterSettings<T>, tolerance),
        offsetof(FixedPointIterSettings<T>, max_iter)
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}




template<std::floating_point T>
[[nodiscard]]
auto create_mpi_stencil_type(MPI_Datatype indexer2d_type) -> MPI_Datatype {
    constexpr int n_fields = 6;
    constexpr int block_lengths[n_fields] = {1, 1, 1, 1, 1, 1};
    const MPI_Datatype types[n_fields] = {
        indexer2d_type,
        get_mpi_type<T>(),  // top
        get_mpi_type<T>(),  // bottom
        get_mpi_type<T>(),  // left
        get_mpi_type<T>(),  // right
        get_mpi_type<T>()   // center
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(ConstantStencil2D<T>, shape),
        offsetof(ConstantStencil2D<T>, m_top),
        offsetof(ConstantStencil2D<T>, m_bottom),
        offsetof(ConstantStencil2D<T>, m_left),
        offsetof(ConstantStencil2D<T>, m_right),
        offsetof(ConstantStencil2D<T>, m_center)
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}

#endif //MPI_UTILS_H
