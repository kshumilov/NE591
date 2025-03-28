#include <concepts>
#include <cstddef>
#include <utility>
#include <algorithm>
#include <complex>
#include <functional>
#include <memory>
#include <chrono>

#include <fmt/core.h>

#include <nlohmann/json.hpp>

#include "methods/linalg/Axb/cg.h"
#include "methods/linalg/Axb/sor.h"

#include "build_system.h"

enum class System
{
    Custom,
    Random,
};


template<>
struct fmt::formatter<System>
{
    constexpr auto parse(const format_parse_context& ctx) { return ctx.begin(); }

    constexpr auto format(const System& sys, format_context& ctx) const
    {
        switch (sys)
        {
            case System::Custom:
                return fmt::format_to(ctx.out(), "custom");
            case System::Random:
                return fmt::format_to(ctx.out(), "random");
            default:
                std::unreachable();
        }
    }
};


template<std::floating_point T>
auto build_system(const int n, const System system)
{
    switch (system)
    {
        case System::Custom:
            return build_custom_system<T>(n);
        case System::Random:
            return build_random_system<T, MatrixSymmetry::Symmetric>(n, -1, 1);
        default:
            std::unreachable();
    }
}

template<std::floating_point T>
struct TimingInfo {
    int n{};
    AxbAlgorithm algo{};
    std::chrono::duration<long long, std::nano> time{};  // nanoseconds
    bool converged{};
    T iterative_error{};
    T residual_error{};
    int iterations{};

    TimingInfo(
        const int n_,
        const std::chrono::duration<long long, std::nano> time_,
        const bool converged_, const IterAxbState<T>& result
    )
    : n{n_} , algo{result.algorithm()}, time{time_}
    , converged{ converged_ }
    , iterative_error{ result.error() }
    , residual_error{ norm_l2(result.residual()) }
    , iterations{ result.iteration() }
    {}

    template<class BasicJsonType>
    friend void to_json(BasicJsonType& j, const TimingInfo& ti) {
        j["n"] = ti.n;
        j["algo"] = std::to_underlying(ti.algo);
        j["time"] = ti.time.count();
        j["iterative_error"] = ti.iterative_error;
        j["residual_error"] = ti.residual_error;
        j["iterations"] = ti.iterations;
        j["converged"] = ti.converged;
    }
};


template<std::floating_point T, class Algo>
auto time(const Algo& algo, std::shared_ptr<const LinearSystem<T>> system)
{
    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = algo.solve(system);
    const auto end = std::chrono::high_resolution_clock::now();

    return TimingInfo<T>{
        system->rank(),
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start),
        result.first, *(result.second),
    };
}


template<std::floating_point T>
auto get_timings(std::span<const int> N, const int k, const System system_type)
{
    constexpr FPSettings<T> fps{1e-8, 10'000};
    constexpr CG<T> cg{fps};
    constexpr SOR<T> sor{fps, SORParams<T>{1.0}};

    std::vector<TimingInfo<T>> timings{};

    for (const auto n : N)
    {
        for ([[maybe_unused]] const auto i : std::views::iota(0, k))
        {
            const auto system = build_system<T>(n, system_type);
            timings.emplace_back(time<T>(cg, system));
            timings.emplace_back(time<T>(sor, system));
        }
        fmt::println("Done {}", n);
    }

    return timings;
}


template<std::floating_point T>
void save_timings(std::span<const TimingInfo<T>> timings, std::string_view filename)
{
    std::ofstream output{ (filename.data()) };
    if (!output.is_open())
        throw std::runtime_error(
            fmt::format("Could not open: '{}'", filename)
        );

    const json j = timings;
    output << std::setw(4) << j << std::endl;
}

using real = long double;

int main()
{
    constexpr auto N = std::to_array({32, 64, 128, 256, 512, 1024, 2048});
    constexpr int k = 20;
    constexpr auto system_type = System::Random;
    const std::string output_filename{
        fmt::format("timing_{}.json", system_type)
    };

    const auto timings = get_timings<real>(N, k, system_type);
    save_timings<real>(timings, output_filename);
    std::exit(EXIT_SUCCESS);
}