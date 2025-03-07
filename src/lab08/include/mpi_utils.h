#ifndef MPI_UTILS_H
#define MPI_UTILS_H

#include <mpi.h>
#include <concepts>
#include <type_traits>

auto get_local_rank(MPI_Comm comm) -> int;
auto get_world_size(MPI_Comm comm) -> int;
auto get_processor_name(MPI_Comm comm) -> int;


// Template function to map C++ floating-point types to MPI floating-point types
template <std::floating_point T>
MPI_Datatype get_mpi_type() {
    static_assert(std::is_floating_point_v<T>, "T must be a floating-point type");
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

#endif //MPI_UTILS_H
