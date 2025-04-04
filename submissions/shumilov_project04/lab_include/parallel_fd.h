#ifndef PARALLEL_FD_H
#define PARALLEL_FD_H

#include <concepts>

#include <mpi.h>

#include "methods/finite_difference.h"
#include "methods/optimize.h"
#include "topology.h"
#include "mpi_utils.h"

template <std::floating_point T>
struct ParallelFiniteDifference
{
    FixedPointIterSettings<T> settings{};
    CartesianTopology topology{};

    [[nodiscard]]
    auto solve(const ConstantStencil2D<T>& stencil, const Matrix<T>& f) const
    {
        constexpr PointJacobiAlgorithm<T> pj{};

        auto result = fixed_point_iteration(
            [&](typename PointJacobiAlgorithm<T>::State& state) constexpr
            {
                state.curr.display();

                MPI_Barrier(MPI_COMM_WORLD);
                const auto local_error = pj.iter(state, stencil, f);

                state.curr.display();

                T global_error{};
                MPI_Allreduce(
                    &local_error,
                    &global_error,
                    1,
                    get_mpi_type<T>(),
                    MPI_MAX,
                    MPI_COMM_WORLD
                );

                MPI_Barrier(MPI_COMM_WORLD);
                this->update_halo(state.curr);

                MPI_Barrier(MPI_COMM_WORLD);
                return global_error;
            },
            pj.init(stencil, f),
            settings
        );

        return pj.finalize(std::move(result), stencil, f);
    }

    auto update_halo(Matrix<T>& u) const
    {
        std::vector<T> halo_y(u.cols() - 2);
        std::vector<T> halo_x(u.rows() - 2);

        // Even - send, Odd - receive
        if (topology.is_red())
        {
            if (topology.neighbors.contains(Direction2D::Top))
            {
                for (std::size_t j{}; j < halo_y.size(); ++j)
                    halo_y[j] = u[u.rows() - 2, j + 1];

                MPI_Send(
                    halo_y.data(),
                    halo_y.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Top),
                    0,
                    MPI_COMM_WORLD
                );
            }

            if (topology.neighbors.contains(Direction2D::Bottom))
            {
                for (std::size_t j{}; j < halo_y.size(); ++j)
                    halo_y[j] = u[1, j + 1];

                MPI_Send(
                    halo_y.data(),
                    halo_y.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Bottom),
                    1,
                    MPI_COMM_WORLD
                );
            }

            if (topology.neighbors.contains(Direction2D::Left))
            {

                for (std::size_t i{}; i < halo_x.size(); ++i)
                    halo_x[i] = u[i + 1, 1];

                MPI_Send(
                    halo_x.data(),
                    halo_x.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Left),
                    2,
                    MPI_COMM_WORLD
                );
            }

            if (topology.neighbors.contains(Direction2D::Right))
            {
                for (std::size_t i{}; i < halo_x.size(); ++i)
                    halo_x[i] = u[i + 1, u.cols() - 2];

                MPI_Send(
                    halo_x.data(),
                    halo_x.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Right),
                    3,
                    MPI_COMM_WORLD
                );
           }
        }
        else
        {
            if (topology.neighbors.contains(Direction2D::Bottom))
            {
                MPI_Recv(
                    halo_y.data(),
                    halo_y.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Bottom),
                    0,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );

                for (std::size_t j{}; j < halo_y.size(); ++j)
                    u[0, j + 1] = halo_y[j];
            }

            if (topology.neighbors.contains(Direction2D::Top))
            {
                MPI_Recv(
                    halo_y.data(),
                    halo_y.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Top),
                    1,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );

                for (std::size_t j{}; j < halo_y.size(); ++j)
                    u[u.rows() - 1, j + 1] = halo_y[j];
            }

            if (topology.neighbors.contains(Direction2D::Right))
            {
                MPI_Recv(
                    halo_x.data(),
                    halo_x.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Right),
                    2,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );

                for (std::size_t i{}; i < halo_y.size(); ++i)
                    u[i + 1, u.cols() - 1] = halo_x[i];
            }

            if (topology.neighbors.contains(Direction2D::Left))
            {
                MPI_Recv(
                    halo_x.data(),
                    halo_x.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Left),
                    3,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );

                for (std::size_t i{}; i < halo_x.size(); ++i)
                    u[i + 1, 0] = halo_x[i];
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        // Even - send, Odd - receive
        if (topology.is_red())
        {
            if (topology.neighbors.contains(Direction2D::Bottom))
            {
                MPI_Recv(
                    halo_y.data(),
                    halo_y.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Bottom),
                    0,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );

                for (std::size_t j{}; j < halo_y.size(); ++j)
                    u[0, j + 1] = halo_y[j];
            }

            if (topology.neighbors.contains(Direction2D::Top))
            {
                MPI_Recv(
                    halo_y.data(),
                    halo_y.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Top),
                    1,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );

                for (std::size_t j{}; j < halo_y.size(); ++j)
                    u[u.rows() - 1, j + 1] = halo_y[j];
            }

            if (topology.neighbors.contains(Direction2D::Right))
            {
                MPI_Recv(
                    halo_x.data(),
                    halo_x.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Right),
                    2,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );

                for (std::size_t i{}; i < halo_x.size(); ++i)
                    u[i + 1, u.cols() - 1] = halo_x[i];
            }

            if (topology.neighbors.contains(Direction2D::Left))
            {
                MPI_Recv(
                    halo_x.data(),
                    halo_x.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Left),
                    3,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );

                for (std::size_t i{}; i < halo_x.size(); ++i)
                    u[i + 1, 0] = halo_x[i];
            }
        }
        else
        {
            if (topology.neighbors.contains(Direction2D::Top))
            {
                for (std::size_t j{}; j < halo_y.size(); ++j)
                    halo_y[j] = u[u.rows() - 2, j + 1];

                MPI_Send(
                    halo_y.data(),
                    halo_y.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Top),
                    0,
                    MPI_COMM_WORLD
                );
            }

            if (topology.neighbors.contains(Direction2D::Bottom))
            {
                for (std::size_t j{}; j < halo_y.size(); ++j)
                    halo_y[j] = u[1, j + 1];

                MPI_Send(
                    halo_y.data(),
                    halo_y.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Bottom),
                    1,
                    MPI_COMM_WORLD
                );
            }

            if (topology.neighbors.contains(Direction2D::Left))
            {
                for (std::size_t i{}; i < halo_x.size(); ++i)
                    halo_x[i] = u[i + 1, 1];

                MPI_Send(
                    halo_x.data(),
                    halo_x.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Left),
                    2,
                    MPI_COMM_WORLD
                );
            }

            if (topology.neighbors.contains(Direction2D::Right))
            {
                for (std::size_t i{}; i < halo_x.size(); ++i)
                    halo_x[i] = u[i + 1, u.cols() - 2];

                MPI_Send(
                    halo_x.data(),
                    halo_x.size(),
                    get_mpi_type<T>(),
                    topology.neighbors.at(Direction2D::Right),
                    3,
                    MPI_COMM_WORLD
                );
           }
        }
    }
};

#endif //PARALLEL_FD_H
