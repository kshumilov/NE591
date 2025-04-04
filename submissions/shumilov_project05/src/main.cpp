#include <cstdlib>
#include <ranges>
#include <utility>
#include <variant>
#include <chrono>

#ifndef NDEBUG
#include <iostream>
#endif

// MPI Includes (assuming an MPI implementation is linked)
#include <mpi.h>

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <argparse/argparse.hpp>

#include "domain.h"
#include "inputs.h"
#include "project.h"

#include "header.h"


struct CMDArgs
{
    using output_t = std::variant<std::reference_wrapper<std::ostream>, std::ofstream>;

    std::string input_filename{};
    std::optional<std::string> output_filename{};
    std::string flux_filename{"FLUX"};

    output_t output;

    [[nodiscard]]
    CMDArgs(const std::string& i, const std::optional<std::string>& o, const std::string& f)
        : input_filename{ i }
        , output_filename{ o }
        , flux_filename{ f }
        , output{ get_output_stream(output_filename) }
    {}

    template<std::floating_point T>
    [[nodiscard]]
    auto read_input_file() const
    {
        return from_file<Inputs<T>>(input_filename);
    }

    template<std::floating_point T>
    void write_flux(const FixedPointResult<T>& result) const
    {
        std::ofstream flux_output{ flux_filename };
        if (!flux_output.is_open())
            throw std::runtime_error(
                fmt::format("Could not open: '{}'", flux_filename)
            ); // Indicate an error occurred

        fmt::println(flux_output, "{:^5s} {:^5s} {:^14s}", "i", "j", "Flux");

        const auto& x = result.x;
        const auto x_cv = x.padded_array_cview();

        for (const auto i : result.x.iter_rows())
            for (const auto j : result.x.iter_internal_cols())
                fmt::println(flux_output, "{:5d} {:5d} {: 14.8e}", i + 1, j + 1, x_cv[i, j]);
    }

    [[nodiscard]]
    static output_t get_output_stream(const std::optional<std::string>& filename)
    {
        if (!filename)
        {
            // If no filename provided, return standard output stream
            return std::ref(std::cout);
        }

        // Try to open the file
        std::ofstream file_stream(*filename, std::ios::out);

        // Check if file was successfully opened
        if (!file_stream.is_open())
        {
            // If file cannot be opened, throw an exception
            throw std::runtime_error(
                fmt::format("Could not open: '{}'", *filename)
            );
        }

        // Return the file stream
        return file_stream;
    }

    [[nodiscard]]
    static auto parse(int argc, char** argv)
    {
        argparse::ArgumentParser program{
            "shumilov_project03",
            "1.0",
            argparse::default_arguments::help,
        };

        program.add_argument("input").help("Path to input file.");
        program.add_argument("-o", "--output").help("Path to output file.");
        program.add_argument("-f", "--flux").help("Path to flux file").default_value("FLUX");

        program.parse_args(argc, argv);

        return CMDArgs{
            program.get<std::string>("input"),
            program.present<std::string>("-o"),
            program.get<std::string>("-f"),
        };
    }
};



using real = long double;
constexpr int MANAGER = 0;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    try {
        const MPIDomain2D domain{MANAGER}; // Sets up rank, size, Cartesian comm

        // Read CMD Arguments on manager process
        auto cmd_args = [&] -> std::optional<CMDArgs>
        {
            if (domain.is_manager())
                return std::make_optional(CMDArgs::parse(argc, argv));
            return std::nullopt;
        }();

        auto inputs = cmd_args.and_then(
            [&](CMDArgs& args)
            {
                const Info header{
                    .title = "NE 501 Project #5",
                    .author = "Kirill Shumilov",
                    .date = "04/04/2025",
                    .description = "Parallel implementation of PJ, GS, and SOR"
                };

                auto i = args.read_input_file<real>();

                std::visit(
                    [&](auto& stream)
                    {
                        fmt::println(
                            stream,
                            "{}{}{}",
                            header, i, domain
                        );
                    },
                    args.output
                );
                return std::make_optional(std::move(i));
            }
        );

        const auto start = std::chrono::high_resolution_clock::now();
        const auto problem = build_problem(std::move(inputs), domain);

        #ifndef NDEBUG
        if (domain.is_manager())
        {
            fmt::println(std::cerr, "Source Blocks:");
        }
        problem.rhs.display(std::cerr, domain);
        #endif

        const auto result = problem.solve(domain);
        const auto end = std::chrono::high_resolution_clock::now();

        if (cmd_args)
        {
            std::visit(
                [&](auto& stream)
                {
                    fmt::println(
                        stream,
                        "{2:^{0}}\n"
                        "{1:-^{0}s}\n"
                        "{3}\n"
                        "{1:=^{0}s}\n"
                        "Execution time: {4:%S} seconds.\n"
                        "{1:=^{0}s}",
                        80, "",
                        "Results",
                        result,
                        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
                    );
                },
                cmd_args->output
            );

            if (result.x.size() > 64)
                cmd_args->write_flux(result);
        }
    }
    catch (const std::exception& err)
    {
        int rank{};
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        fmt::print(
            std::cerr,
            "\n{}: {}\n\n",
            fmt::format(
                fmt::emphasis::bold | fg(fmt::color::red),
                "Rank {}: Error: ", rank
            ),
            err.what()
        );

        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}