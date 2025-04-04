//
// Created by Kirill Shumilov on 4/2/25.
//

#include "mpi_types.h"
#include "grid.h"


[[nodiscard]]
auto create_mpi_shape_type() -> MPI_Datatype {
    constexpr int n_fields = 2;
    constexpr int block_lengths[n_fields] = {1, 1};
    constexpr MPI_Datatype types[n_fields] = {
        MPI_INT,
        MPI_INT,
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(Shape2D, m_rows),
        offsetof(Shape2D, m_cols),
    };

    auto mpi_type{ MPI_DATATYPE_NULL };
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}


[[nodiscard]]
auto create_mpi_grid_type(MPI_Datatype shape_dt) -> MPI_Datatype {
    constexpr int n_fields = 1;
    constexpr int block_lengths[n_fields] = {1};
    const MPI_Datatype types[n_fields] = {
        shape_dt,
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(Grid2D, shape)
    };

    auto mpi_type{ MPI_DATATYPE_NULL };
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}




