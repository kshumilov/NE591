#ifndef INLAB08_H
#define INLAB08_H

#include <chrono>
#include <type_traits>

#include "methods/optimize.h"

#include "flux.h"




template<std::floating_point T>
struct inlab08
{
    Slab<T> slab{};
    FixedPointIterSettings<T> iter_settings{};

    struct Solution
    {
        std::shared_ptr<inlab08> lab{};
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
            std::make_shared<inlab08>(*this),
            std::move(flux),
            std::move(result),
            duration
        };
    }

    [[nodiscard]]
    auto init_flux() -> Flux<T>
    {
        return slab.init_flux();
    }

    [[nodiscard]]
    auto source_iteration(Flux<T>& flux) const -> FixedPointIterResult<std::span<T>>
    {
        std::vector<T> scalar_flux_next = flux.slab->init_scalar_flux();

        auto g = [&](std::span<const T> scalar_flux_curr) -> std::span<T>
        {
            flux.run_source_iteration(scalar_flux_curr, scalar_flux_next);
            std::swap(flux.scalar_flux, scalar_flux_next);
            return std::span{flux.scalar_flux};
        };

        return fixed_point_iteration<std::span<T>>(
            g,
            std::span{flux.scalar_flux},
            max_rel_diff<std::span<const T>, std::span<const T>>,
            iter_settings
        );
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
    static auto from_file(std::istream& input)
    {
        return inlab08{
            Slab<T>::from_file(input),
            FixedPointIterSettings<T>::from_file(input)
        };
    }
};


#endif //INLAB08_H
