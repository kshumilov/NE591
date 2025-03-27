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

        auto [A, b] = build_random_system<real, MatrixSymmetry::Symmetric>(64);
        make_diag_dom(A);

        fmt::println("A, {}:\n{}", A.shape_info(), A.to_string());

        constexpr FPSettings<real> fps{1e-8, 10'000};

        const auto cg = std::make_unique<CGState<real>>(fps, A, b);
        const auto cg_converged = cg->solve();
        fmt::println("CG ({}, {}):\n{}\n", cg_converged, static_cast<FixedPoint<real>>(*cg), cg->x);

        const auto pj = std::make_unique<PJState<real>>(fps, A, b);
        const auto pj_converged = pj->solve();
        fmt::println("PJ ({}, {}):\n{}\n", pj_converged, static_cast<FixedPoint<real>>(*pj), pj->x);

        const auto sor = std::make_unique<SORState<real>>(fps, A, b, 1.5);
        const auto sor_converged = sor->solve();
        fmt::println("SOR({}, {}):\n{}\n", sor_converged, static_cast<FixedPoint<real>>(*sor), sor->x);

        const auto [P, success] = lup_factor_inplace<real>(A);
        const auto x = lup_solve<real>(A, P, b);
        std::vector<real> tmp(x.size());
        std::vector<real> residual{ b.cbegin(), b.cend() };
        gemv<real, MatrixSymmetry::Upper>(A, x, tmp, real{ 1 });
        gemv<real, MatrixSymmetry::Lower, Diag::Unit>(A, tmp, residual, real{ -1 }, real{ 1 });
        const auto error = max_abs(residual);
        fmt::println("LUP({}):\n{}\n", error, sor->x);



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
