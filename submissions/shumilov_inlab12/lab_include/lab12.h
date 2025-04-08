#ifndef INLAB12_H
#define INLAB12_H

#include <algorithm>
#include <chrono>
#include <concepts>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>

#include <argparse/argparse.hpp>

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "methods/linalg/eig/power_iter.h"
#include "methods/linalg/utils/io.h"

#include "lab/io.h"
#include "lab/lab.h"


template<std::floating_point T>
struct Inlab12
{
    EigenValueUpdate algorithm{EigenValueUpdate::PowerIteration};
    FPSettings<T> iter_settings{};
    std::shared_ptr<Matrix<T>> A{};
    std::vector<T> eigenvector{};
    T eigenvalue{1.};

    [[nodiscard]]
    auto run()
    {
        const PowerIteration<T> pi {iter_settings, algorithm};
        return pi.solve(A, std::move(eigenvector));
    }

    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
        const auto algo = read_eig_update_algorithm(input);

        const T lambda_guess = [&]
        {
            if (const auto l = read_value<T>(input); l.has_value())
                return l.value();

            throw std::runtime_error(fmt::format("Could not read `{}`", "lambda"));
        }();

        auto settings = FPSettings<T>::from_file(input);
        const auto rank = read_positive_value<int>(input, "Matrix rank");
        return Inlab12{
            algo,
            std::move(settings),
            std::make_shared<Matrix<T>>(read_square_matrix<T>(input, static_cast<std::size_t>(rank))),
            read_vector<T>(input, static_cast<std::size_t>(rank)),
            lambda_guess,
        };
    }
};


template<std::floating_point T>
struct fmt::formatter<Inlab12<T>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    [[nodiscard]]
    auto format(const Inlab12<T>& inputs, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "{}\n"
            "Matrix A, {:F: 14.8e}\n"
            "Eigenvalue update policy: {}\n"
            "Eigenvalue guess: {:14.8e}\n"
            "Eigenvector guess, x:\n{:: 14.8e}",
            inputs.iter_settings, *inputs.A,
            inputs.algorithm,
            inputs.eigenvalue,
            inputs.eigenvector
        );
    }
};


template<std::floating_point T>
struct Project
{
    Info info{
        .title = "NE 501 Inlab #12",
        .author = "Kirill Shumilov",
        .date = "04/04/2025",
        .description = "Power iteration with Rayleigh Quotien"
    };

    [[nodiscard]]
    Project() = default;

    [[nodiscard]]
    auto run(int argc, char* argv[]) const
    {
        argparse::ArgumentParser program{
            "shumilov_outlab11",
            "1.0",
            argparse::default_arguments::help,
        };
        program.add_argument("input").help("Path to input file.");
        program.add_argument("-o", "--output").help("Path to output file");

        try
        {
            program.parse_args(argc, argv);

            auto output = get_output_stream(
                program.present<std::string>("--output")
            );

            auto lab = read_input_file<Inlab12<T>>(
                program.get<std::string>("input")
            );

            std::visit(
                [&](auto& stream) { this->run(stream, lab); },
                output
            );
        }
        catch (const std::exception& err)
        {
            fmt::print(
                std::cerr,
                "\n{}: {}\n\n",
                fmt::format(
                    fmt::emphasis::bold | fg(fmt::color::red),
                    "Error: "
                ),
                err.what()
            );
            std::exit(EXIT_FAILURE);
        }

        return EXIT_SUCCESS;
    }

    [[nodiscard]]
    auto run(std::ostream& output, Inlab12<T>& lab) const
    {
        fmt::print(output, "{}", info);

        fmt::println(
            output,
            "{1:^{0}s}\n"
            "{2:-^{0}s}\n"
            "{3}",
            COLUMN_WIDTH, "Inputs", "",
            lab
        );

        const auto start = std::chrono::high_resolution_clock::now();
        const auto result = lab.run();
        const auto end = std::chrono::high_resolution_clock::now();

        fmt::println(
            output,
            "{2:=^{0}s}\n"
            "{1:^{0}}\n"
            "{2:-^{0}s}\n"
            "{3}",
            COLUMN_WIDTH, "Results", "",
            result
        );

        fmt::println(
            output,
            "{1:=^{0}s}\n"
            "Execution time: {2:%S} seconds.\n"
            "{1:=^{0}s}",
            COLUMN_WIDTH, "",
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
        );
    }
};

#endif // INLAB12_H
