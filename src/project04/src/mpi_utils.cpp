#include <stdexcept>
#include <memory>

#include <fmt/core.h>

#include "mpi_utils.h"


[[nodiscard]]
auto get_local_rank(MPI_Comm comm) -> int
{
    int rank{};
    if (const auto error_code = MPI_Comm_rank(comm, &rank);
        MPI_SUCCESS != error_code)
    {
        throw std::runtime_error(
            fmt::format("MPI_Comm_rank failed: {}", error_code)
        );
    }
    return rank;
}


[[nodiscard]]
auto get_world_size(MPI_Comm comm) -> int
{
    int world_size{};
    if (const auto error_code = MPI_Comm_size(comm, &world_size);
        MPI_SUCCESS != error_code)
    {
        throw std::runtime_error(
            fmt::format("MPI_Comm_size failed: {}", error_code)
        );
    }
    return world_size;
}


[[nodiscard]]
auto get_processor_name() -> std::string
{
    const auto name = std::make_unique<char[]>(MPI_MAX_PROCESSOR_NAME);
    auto size = MPI_MAX_PROCESSOR_NAME;
    // add error checking
    if (const auto error_code = MPI_Get_processor_name(name.get(), &size);
        MPI_SUCCESS != error_code)
    {
        throw std::runtime_error(
            fmt::format("MPI_Get_processor_name failed: {}", error_code)
        );
    }
    return std::string{ name.get() };
}


[[nodiscard]]
auto create_mpi_indexer2d_type() -> MPI_Datatype {
    constexpr int n_fields = 2;
    constexpr int block_lengths[n_fields] = {1, 1};
    const MPI_Datatype types[n_fields] = {
        MPI_INT,
        MPI_INT,
    };

    constexpr MPI_Aint offsets[n_fields] = {
        offsetof(Indexer2D<>, m_rows),
        offsetof(Indexer2D<>, m_cols)
    };

    MPI_Datatype mpi_type;
    MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_type);
    MPI_Type_commit(&mpi_type);

    return mpi_type;
}
