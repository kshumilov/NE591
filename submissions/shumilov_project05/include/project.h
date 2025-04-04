//
// Created by Kirill Shumilov on 4/3/25.
//

#ifndef PROJECT_H
#define PROJECT_H

#include <concepts>

#include "block.h"
#include "domain.h"
#include "inputs.h"

#include "point_jacobi.h"
#include "sor.h"

#include "mpi_types.h"

template<std::floating_point T>
struct DistributedProblem
{
    SolverConfig<T> config{};
    Stencil2D<T> stencil{};
    Distributed2DBlock<T> rhs{};

    [[nodiscard]]
    DistributedProblem(
        const SolverConfig<T>& config_,
        const Stencil2D<T>& stencil_,
        Distributed2DBlock<T>&& rhs_
    )
        : config{ config_ }
        , stencil{ stencil_ }
        , rhs{ std::move(rhs_) }
    {}

    [[nodiscard]]
    auto solve(const MPIDomain2D& domain) const
    {
        auto x = Distributed2DBlock<T>::zeros_like(domain, rhs, Padding{ 1 });

        auto result = [&]
        {
            switch (config.algorithm)
            {
                case Algorithm::PointJacobi:
                {
                    return point_jacobi<T>(
                        std::move(x),
                        stencil,
                        rhs,
                        config.settings,
                        domain
                    );
                }
                case Algorithm::GaussSeidel:
                case Algorithm::SuccessiveOverRelaxation:
                {
                    return sor<T>(
                        std::move(x),
                        stencil,
                        rhs,
                        config.relaxation_factor,
                        config.settings,
                        domain
                    );
                }
                default:
                    throw std::invalid_argument("Invalid algorithm");
            }
        }();

        result.x.gather(domain, domain.manager);

        return result;
    }
};


template<std::floating_point T>
[[nodiscard]]
auto build_problem(std::optional<Inputs<T>>&& inputs, const MPIDomain2D& domain)
{
    Shape2D shape{};
    SolverConfig<T> config{};
    Stencil2D<T> stencil{};

    const MPIHelperTypes<T> dts{};
    std::vector<T> source{}; // Only populated on the root

    if (inputs.has_value())
    {
        stencil = inputs->build_stencil();
        shape = inputs->inner_grid.shape;
        config = inputs->solver_config;
        std::swap(source, inputs->source);
    }

    MPI_Bcast(&stencil, 1, dts.stencil, domain.manager, domain.cart_comm);
    MPI_Bcast(&shape, 1, dts.shape, domain.manager, domain.cart_comm);
    MPI_Bcast(&config, 1, dts.config, domain.manager, domain.cart_comm);

    return DistributedProblem<T>{
        config,
        stencil,
        Distributed2DBlock<T>::scatter(domain, shape, source, domain.manager)
    };
}


#endif //PROJECT_H
