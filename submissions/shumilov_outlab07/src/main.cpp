#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <cmath>
#include <mpi.h>

#include <fmt/color.h>
#include <fmt/ostream.h>

#include <argparse/argparse.hpp>

#include <concepts>


using real = long double;


[[nodiscard]]
auto get_local_rank(const MPI_Comm comm) -> int
{
    int rank{};
    if (const auto error_code = MPI_Comm_rank(comm, &rank);
        MPI_SUCCESS != error_code)
    {
        throw std::runtime_error(fmt::format("MPI_Comm_rank failed: {}", error_code));
    }
    return rank;
}


[[nodiscard]]
auto get_world_size(const MPI_Comm comm) -> int
{
    int world_size{};
    if (const auto error_code = MPI_Comm_size(comm, &world_size);
        MPI_SUCCESS != error_code)
    {
        throw std::runtime_error(fmt::format("MPI_Comm_size failed: {}", error_code));
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
        throw std::runtime_error(fmt::format("MPI_Get_processor_name failed: {}", error_code));
    }
    return std::string{ name.get() };
}


[[nodiscard]]
constexpr auto is_power_of_two(const int n) -> bool
{
    if (n <= 0)
    {
        return false;
    }

    int power{};
    int k{ 1 };
    while (n > k)
    {
        k *= 2;
        power += 1;
    }

    return n == k;
}


template<std::floating_point T>
constexpr auto compute_partial_series(const int start = int{ 1 }, const int stop = int{ 1 }) -> T
{
    if (start < 1)
    {
        throw std::invalid_argument(fmt::format("`start` must be a positive integer: {}", start));
    }

    if (stop < start)
    {
        throw std::invalid_argument(fmt::format("`stop` must be equal or greater than `start`: {} > {}", stop, start));
    }

    T result{};
    for (int i{ start }; i < stop; ++i)
    {
        const T i_{ static_cast<T>(i) };
        const T tmp{ (1 + std::log10(i_)) / (i_ + std::log10(i_ + 1.0)) };
        result += tmp * tmp;
    }
    return result;
}


[[nodiscard]]
auto get_interval(const int i, const int n, const int num_proc) -> std::pair<int, int>
{
    if (i < 0)
    {
        throw std::invalid_argument(fmt::format("`i` must be a non-negative integer: {}", i));
    }

    if (i >= n)
    {
        throw std::invalid_argument(fmt::format("`n` must be greater than `i`: {} >= {}", i, n));
    }

    if (num_proc < 1)
    {
        throw std::invalid_argument(fmt::format("`num_proc` must be a non-negative integer: {}", num_proc));
    }

    const auto batch_size = static_cast<int>(std::ceil(static_cast<real>(n) / num_proc));
    const auto start{ std::min(i * batch_size, n) };
    const auto stop{ std::min((i + 1) * batch_size, n) };

    return { start, stop };
}


[[nodiscard]]
auto read_n(const std::string& filename) -> int {
    int n{};

    std::ifstream file{ filename };
    if (not file.is_open()) {
        throw std::runtime_error(fmt::format("Could not open file {}", filename));
    }

    if (file >> n) {
        file.close();

        if (n < 1)
        {
            throw std::runtime_error(fmt::format("`n` must be positive: {}", n));
        }
        return n;
    }

    file.close();
    throw std::runtime_error(fmt::format("Could not read `{}`", "n"));
}


auto mpi_reduce(const auto n, const MPI_Comm comm) -> real
{
    const auto rank = get_local_rank(comm);
    const auto num_proc = get_world_size(comm);
    const auto [start, stop] = get_interval(rank, n, num_proc);
    auto sum = compute_partial_series<real>(start + 1, stop + 1);

    #ifndef NDEBUG
    fmt::println("Rank {}: [{}, {}) = {}", rank, start, stop, sum);
    #endif

    for (int stride{ 2 }; stride <= num_proc; stride *= 2)
    {
        const int offset{ stride / 2 };

        if (rank % stride == 0)
        {
            real message{};
            const int source{ rank + offset };

            #ifndef NDEBUG
            fmt::println("{} <- {}", rank, source);
            #endif

            MPI_Recv(&message, 1, MPI_LONG_DOUBLE, source, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += message;
        }

        if (rank % stride == offset)
        {
            const auto message{ sum };
            const int target{ rank - offset };

            #ifndef NDEBUG
            fmt::println("{} -> {}", rank, target);
            #endif

            MPI_Send(&message, 1, MPI_LONG_DOUBLE, target, stride, MPI_COMM_WORLD);
            break;
        }
    }

    #ifndef NDEBUG
    if (rank == 0)
    {
        const auto true_sum = compute_partial_series<real>(1, n + 1);
        fmt::println(std::cerr, "Error: {}", std::abs(sum - true_sum));
    }
    #endif

    return sum;
}


auto main(int argc, char* argv[]) -> int
{
    if (const auto error_code = MPI_Init(&argc, &argv);
        MPI_SUCCESS != error_code)
    {
        throw std::runtime_error(fmt::format("MPI_Init failed: {}", error_code));
    }

    try
    {
        const auto n = read_n("input");

        const int num_proc = get_world_size(MPI_COMM_WORLD);
        if (not is_power_of_two(num_proc))
        {
            throw std::runtime_error(
                fmt::format("Number of MPI processes must be be equal to a power of two: {}", num_proc)
            );
        }

        const auto start_time = std::chrono::high_resolution_clock::now();
        const auto sum = mpi_reduce(n, MPI_COMM_WORLD);
        const auto end_time = std::chrono::high_resolution_clock::now();

        if (const auto rank = get_local_rank(MPI_COMM_WORLD);
            rank == 0)
        {
            const std::string output_filename{ fmt::format("output.{:d}", rank) };
            std::ofstream output{ output_filename };
            if (not output.is_open())
            {
                throw std::runtime_error(
                    fmt::format("Could not open file {}", output_filename)
                );
            }

            fmt::println(
                output,
                "===========================================================\n"
                "This program was developed by Kirill Shumilov on 02/28/2025\n"
                "Function: Spanning Tree Reduction.\n"
                "===========================================================\n"
                "Rank: {}\n"
                "Host: {}\n"
                "-----------------------------------------------------------\n"
                "n = {:d}\n"
                "#processes = {:d}\n"
                "sum = {:12.6e}\n"
                "-----------------------------------------------------------\n"
                "Execution time: {:%S} seconds.\n"
                "===========================================================",
                rank, get_processor_name(),
                n, num_proc, sum,
                std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time)
            );

            output.close();
        }

        MPI_Finalize();
    }
    catch (const std::exception& err)
    {
        fmt::print(
            std::cerr,
            "\n{}: {}\n\n",
            fmt::format(
                fmt::emphasis::bold | fg(fmt::color::red),
                "Error (Rank {}): ",
                get_local_rank(MPI_COMM_WORLD)
            ),
            err.what()
        );

        MPI_Finalize();
        std::exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
