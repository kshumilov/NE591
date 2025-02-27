#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#include <mpi.h>

#include <fmt/color.h>
#include <fmt/ostream.h>


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


auto print_header(std::ostream& out) -> void
{
    fmt::println(
        out,
        "===========================================================\n"
        "This program was developed by Kirill Shumilov on 02/21/2025\n"
        "Function: parallel version of the hello_world code.\n"
        "==========================================================="
    );
}


[[nodiscard]]
auto read_line_from_file(const std::string& filename) -> std::string
{
    std::string line{};

    std::ifstream file{ filename };
    if (not file.is_open())
    {
        throw std::runtime_error(fmt::format("Could not open file {}", filename));
    }

    std::getline(file, line);
    file.close();
    return line;
}


auto hello_world(std::ostream& out) -> void
{
    const auto name = read_line_from_file("input");

    // Get the rank of the process
    const auto local_rank = get_local_rank(MPI_COMM_WORLD);

    // Get the name of the processor
    const auto hostname = get_processor_name();

    fmt::println(
        out,
        "Hello {:s} from MPI Task {:d} on host {:s}",
        name,
        local_rank,
        hostname
    );
}


auto main(int argc, char* argv[]) -> int
{
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    try
    {
        const auto start = std::chrono::high_resolution_clock::now();
        const auto local_rank = get_local_rank(MPI_COMM_WORLD);

        const std::string output_filename{ fmt::format("output.{:d}", local_rank) };

        std::ofstream output{ output_filename };
        if (not output.is_open())
        {
            throw std::runtime_error(
                fmt::format("Could not open file {}", output_filename)
            );
        }

        if (local_rank == 0)
        {
            print_header(output);
        }

        hello_world(output);

        const auto end = std::chrono::high_resolution_clock::now();

        fmt::println(
            output,
            "\n"
            "Execution time: {:%S} seconds.\n"
            "===========================================================",
            std::chrono::duration_cast<std::chrono::microseconds>(end - start)
        );

        output.close();
    }
    catch (const std::exception& err)
    {
        // std::cerr << "\n" << format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ") << err.what() << "\n\n";
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

    MPI_Finalize();
    return EXIT_SUCCESS;
}
