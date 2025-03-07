#ifndef LAB08_MPI_H
#define LAB08_MPI_H

#include "cassert"

#include <mpi.h>

#include "inlab08.h"
#include "mpi_utils.h"


template<std::floating_point T>
struct Outlab08
{
    Slab<T> slab{};
    FixedPointIterSettings<T> iter_settings{};

    struct Solution
    {
        std::shared_ptr<Outlab08> lab{};
        Flux<T> flux{};
        FixedPointIterResult<std::span<T>> result{};
        std::chrono::duration<long long, std::nano> time{}; // nanoseconds

        auto echo(std::ostream& output) const
        {
            lab->echo(output);
            fmt::println(output, "{}\n{}", result.to_string(), dash_divider);
            fmt::println(output, "{}\n{}", flux.to_string(), dash_divider);
            fmt::println(
                output,
                "Execution time: {:%S} seconds.\n{}",
                time,
                eq_divider
            );
        }
    };

    [[nodiscard]]
    auto run() -> Solution
    {
        Flux flux = init_flux();
        const auto start = std::chrono::high_resolution_clock::now();
        const auto result = source_iteration(flux);
        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return {
            std::make_shared<Outlab08>(*this),
            std::move(flux),
            std::move(result),
            duration
        };
    }
    [[nodiscard]]
    auto init_flux() -> Flux<T>
    {
        const int k_min = get_local_rank(MPI_COMM_WORLD) * angular_batch_size() + 1;
        return this->slab.init_flux(k_min, angular_batch_size());
    }

    [[nodiscard]]
    constexpr auto angular_batch_size() const -> int
    {
        const auto nproc = get_world_size(MPI_COMM_WORLD);
        assert(this->slab.grid.num_angular_points % nproc == 0);
        return this->slab.grid.num_angular_points / nproc;
    }

    [[nodiscard]]
    auto source_iteration(Flux<T>& flux) const -> FixedPointIterResult<std::span<T>>
    {
        std::vector<T> scalar_flux_local = flux.slab->init_scalar_flux();
        std::vector<T> scalar_flux_global = flux.slab->init_scalar_flux();

        auto g = [&](std::span<const T> scalar_flux_curr) -> std::span<T>
        {
            flux.run_source_iteration(scalar_flux_curr, scalar_flux_local);
            MPI_Barrier(MPI_COMM_WORLD);

            MPI_Allreduce(
                scalar_flux_local.data(),
                scalar_flux_global.data(),
                scalar_flux_global.size(),
                get_mpi_type<T>(),
                MPI_SUM,
                MPI_COMM_WORLD
            );

            std::swap(flux.scalar_flux, scalar_flux_global);
            return std::span{flux.scalar_flux};
        };

        return fixed_point_iteration<std::span<T>>(
            g,
            std::span{flux.scalar_flux},
            max_rel_diff<std::span<const T>, std::span<const T>>,
            this->iter_settings
        );
    }

    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
        return Outlab08{
            Slab<T>::from_file(input),
            FixedPointIterSettings<T>::from_file(input)
        };
    }

    auto echo(std::ostream& output) const -> void
    {
        fmt::println(
            output,
            "{}\n{}"
            "\n{}\n{}",
            slab.to_string(),
            dash_divider,
            iter_settings.to_string(),
            dash_divider
        );
    }

    [[nodiscard]]
    static auto read_input_file(const std::string& filename)
    {
        const auto input_filepath = std::filesystem::path{ filename };

        if (input_filepath.empty())
        {
            throw std::runtime_error(
                fmt::format("Input file does not exist: {}", input_filepath.string())
            ); // Indicate an error occurred
        }

        std::ifstream input{ input_filepath };

        if (!input.is_open())
        {
            throw std::runtime_error(
                fmt::format("Could not open input file: {}", input_filepath.string())
            ); // Indicate an error occurred
        }

        try
        {
            const auto p = Outlab08::from_file(input);
            input.close();
            return p;
        }
        catch (const std::exception&)
        {
            input.close();
            throw;
        }
    }
};

#endif //LAB08_MPI_H
