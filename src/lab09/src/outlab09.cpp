#include <cstdlib>
#include <filesystem>
#include <iostream>

#include <argparse/argparse.hpp>
#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/ostream.h>
#include <methods/linalg/Axb/point_jacobi.h>
#include <methods/linalg/Axb/sor.h>

#include "lab/lab.h"
#include "lab/utils/io.h"

#include "Lab10.h"
#include "custom_system.h"


using real = long double;


int main(int argc, char *argv[])
{
    argparse::ArgumentParser program{
        "shumilov_outlab09",
        "1.0",
        argparse::default_arguments::help,
    };

    program.add_argument("input").help("Path to input file.");
    program.add_argument("-o", "--output").help("Path to output file");

    try
    {
        program.parse_args(argc, argv);

        const auto input_filename = program.get<std::string>("input");
        const auto lab = read_input_file<Lab10<real>>(input_filename);
        const auto result = lab.run();

        // constexpr int n{256};
        // constexpr FPSettings<real> fps{1e-8, 10'000};
        // const auto linear_system = build_random_system<real, MatrixSymmetry::Symmetric>(n, 0, 1);
        // const auto linear_system = build_custom_system<real>(n);

        // const CG<real> cg{ fps };
        // auto cgr = cg.solve(linear_system);
        // fmt::println("{}", cg);
        // fmt::println("{:r:: .3e}", *cgr.second);
        //
        // const SOR<real> sor{fps};
        // auto sorr = sor.solve(linear_system);
        // fmt::println("{:r:: .3e}", *sorr.second);

        if (const auto output_filename = program.present<std::string>("--output");
            output_filename.has_value())
        {
            std::ofstream output{ output_filename.value() };
            if (!output.is_open())
            {
                throw std::runtime_error(
                    fmt::format("Could not open: '{}'", output_filename.value())
                ); // Indicate an error occurred
            }

            fmt::println(
                output,
                "{}\n",
                lab
            );

            Lab10<real>::print_result(output, result);
        }
        else
        {
            fmt::println(
                std::cout,
                "{}",
                lab
            );

            Lab10<real>::print_result(std::cout, result);
        }
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
