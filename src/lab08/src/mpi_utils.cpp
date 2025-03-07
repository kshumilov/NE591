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

