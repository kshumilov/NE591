#ifndef UTILS_H
#define UTILS_H

#include <utility>

#include <mpi.h>


template<class T>
[[nodiscard]]
constexpr MPI_Datatype get_mpi_type()
{
    if constexpr (std::is_same_v<T, float>)
    {
        return MPI_FLOAT;
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        return MPI_DOUBLE;
    }
    else if constexpr (std::is_same_v<T, long double>)
    {
        return MPI_LONG_DOUBLE;
    }
    else if constexpr (std::is_same_v<T, int>)
    {
        return MPI_INT;
    }
    else
    {
        std::unreachable();
    }
}


#endif //UTILS_H
