#include <cstdlib>
#include <stdexcept>
#include <iostream>

#include <argparse/argparse.hpp>
#include <fmt/core.h>
#include <fmt/color.h>

#include "methods/legendre.h"

using real = long double;

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program(
        "gauss_quad",
        "1.0"
    );

    program.add_description(
        "Generates Gauss-Legendre quadrature nodes and weights,\n"
        "derived from Legendre Polynomial of degree n"
    );

    program.add_argument("n")
           .help("Number of nodes to generate; n > 0")
           .scan<'d', int>();

    program.add_argument("--tol")
           .help("Newton-Raphson absolute tolerance: eps > 0")
           .default_value(real{1e-12})
           .scan<'g', real>();

    program.add_argument("--iter")
           .help("Maximum number of iterations: N > 0")
           .default_value(10)
           .scan<'d', int>();

    try
    {
        program.parse_args(argc, argv);

        const auto n = program.get<int>("n");
        if (n < 1)
        {
            throw std::runtime_error(
                fmt::format("Number of nodes, `n`, must be greater than 0: {}", n)
            );
        }

        const auto tol = program.get<real>("--tol");
        const auto iters = program.get<int>("--iter");
        const FixedPointIterSettings<real> settings{tol, iters};

        const auto quad = gauss_legendre_quadrature<real>(n, settings);
        print_gauss_legendre_quadrature(quad, std::cout);

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
        std::cerr << program << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
