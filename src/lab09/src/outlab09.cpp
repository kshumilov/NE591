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

#include "inputs.h"
#include "custom_system.h"


using real = long double;





int main(int argc, char *argv[])
{
    const Info info{
        .title = "NE 501 Outlab #09",
        .author = "Kirill Shumilov",
        .date = "03/21/2025",
        .description = "Preparation for implementation of CG solver for Ax=b systems"
    };

    fmt::print("{}", info);

    // argparse::ArgumentParser program{
    //     "shumilov_outlab09",
    //     "1.0",
    //     argparse::default_arguments::help,
    // };
    //
    // program.add_argument("input").help("Path to input file.");
    // program.add_argument("-o", "--output").help("Path to output file");

    try
    {
        // program.parse_args(argc, argv);

        // const auto input_filename = program.get<std::string>("input");
        // const auto inputs = Inputs<real>::from_file(input_filename);
        //

        constexpr int n{64};
        constexpr FPSettings<real> fps{1e-8, 10'000};
        // const auto linear_system = build_random_system<real, MatrixSymmetry::Symmetric>(n, -1, 1);
        const auto linear_system = build_custom_system<real>(n);

        fmt::println("A{:F: 5.3f}", linear_system->A);

        constexpr CG<real> cg{ fps };
        auto cgr = cg.solve(linear_system);
        fmt::println("{:F:: .3e}", *cgr.second);

        constexpr SOR<real> sor{fps};
        auto sorr = sor.solve(linear_system);
        fmt::println("{:s:: .3e}", *sorr.second);


       //  if (const auto output_filename = program.present<std::string>("--output");
       //      output_filename.has_value())
       //  {
       //      std::ofstream output{ output_filename.value() };
       //      if (!output.is_open())
       //      {
       //          throw std::runtime_error(
       //              fmt::format("Could not open: '{}'", output_filename.value())
       //          ); // Indicate an error occurred
       //      }
       //
       //      fmt::print(
       //          output,
       //          "{}{}\n",
       //          info.to_string(),
       //          inputs.to_string()
       //      );
       //  } else
       //  {
       //      fmt::print(
       //          std::cout,
       //          "{}{}\n",
       //          info.to_string(),
       //          inputs.to_string()
       //      );
       // }
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
