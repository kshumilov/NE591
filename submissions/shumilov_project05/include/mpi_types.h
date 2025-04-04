#ifndef MPI_TYPES_H
#define MPI_TYPES_H

#include <concepts>
#include <domain.h>
#include <map>
#include <type_traits>

#include <mpi.h>

#include "config.h"
#include "neighborhood.h"
#include "shape.h"
#include "stencil.h"

#include "utils.h"


auto create_mpi_shape_type() -> MPI_Datatype;

auto create_mpi_grid_type(MPI_Datatype) -> MPI_Datatype;


template<class T>
[[nodiscard]]
auto create_mpi_neighborhood_type() -> MPI_Datatype {
    constexpr int n_fields = 4;
    constexpr int block_lengths[n_fields] = {1, 1, 1, 1};
    const MPI_Datatype types[n_fields] = {
        get_mpi_type<T>(),  // north
        get_mpi_type<T>(),  // south
        get_mpi_type<T>(),  // east
        get_mpi_type<T>(),  // west
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(Neighborhood2D<T>, north),
        offsetof(Neighborhood2D<T>, south),
        offsetof(Neighborhood2D<T>, east),
        offsetof(Neighborhood2D<T>, west),
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}


template<std::floating_point T>
[[nodiscard]]
auto create_mpi_iter_settings_type() -> MPI_Datatype {
    constexpr int n_fields = 2;
    constexpr int block_lengths[n_fields] = {1, 1};
    const MPI_Datatype types[n_fields] = {
        MPI_INT,
        get_mpi_type<T>(),
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(FixedPointSettings<T>, max_iter),
        offsetof(FixedPointSettings<T>, tolerance),
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}

template<std::floating_point T>
[[nodiscard]]
auto create_mpi_stencil_type(MPI_Datatype neighborhood_dt) -> MPI_Datatype {
    constexpr int n_fields = 2;
    constexpr int block_lengths[n_fields] = {1, 1};
    const MPI_Datatype types[n_fields] = {
        get_mpi_type<T>(),  // center
        neighborhood_dt,
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(Stencil2D<T>, center),
        offsetof(Stencil2D<T>, neighborhood),
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}


template<std::floating_point T>
[[nodiscard]]
auto create_mpi_config_type(MPI_Datatype settings_dt) -> MPI_Datatype
{
    constexpr int n_fields = 3;
    constexpr int block_lengths[n_fields] = {1, 1, 1};
    const MPI_Datatype types[n_fields] = {
        get_mpi_type<int>(),
        settings_dt,
        get_mpi_type<T>(),
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(SolverConfig<T>, algorithm),
        offsetof(SolverConfig<T>, settings),
        offsetof(SolverConfig<T>, relaxation_factor),
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}


template<class T>
struct MPIHelperTypes
{
    MPI_Datatype shape{ MPI_DATATYPE_NULL };
    MPI_Datatype settings{ MPI_DATATYPE_NULL };
    MPI_Datatype grid{ MPI_DATATYPE_NULL };
    MPI_Datatype neighborhood{ MPI_DATATYPE_NULL };
    MPI_Datatype stencil{ MPI_DATATYPE_NULL };
    MPI_Datatype config{ MPI_DATATYPE_NULL };

    [[nodiscard]]
    MPIHelperTypes()
        : shape{ create_mpi_shape_type() }
        , settings{ create_mpi_iter_settings_type<T>() }
        , grid{ create_mpi_grid_type(shape) }
        , neighborhood{ create_mpi_neighborhood_type<T>() }
        , stencil{ create_mpi_stencil_type<T>(neighborhood) }
        , config{ create_mpi_config_type<T>(settings) }
    {}

    MPIHelperTypes(const MPIHelperTypes&) = delete;
    MPIHelperTypes& operator=(const MPIHelperTypes&) = delete;
    MPIHelperTypes(MPIHelperTypes&&) = delete;
    MPIHelperTypes& operator=(MPIHelperTypes&&) = delete;

    ~MPIHelperTypes()
    {
        MPI_Type_free(&shape);
        MPI_Type_free(&settings);
        MPI_Type_free(&grid);
        MPI_Type_free(&neighborhood);
        MPI_Type_free(&stencil);
        MPI_Type_free(&config);
    }
};

#endif //MPI_TYPES_H
