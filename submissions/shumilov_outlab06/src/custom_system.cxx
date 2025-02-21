#include <cstddef>
#include <string>

#include <fmt/color.h>
#include <argparse/argparse.hpp>

#include "lab06.h"
#include "custom_system.h"

using real = long double;

std::size_t validate_matrix_rank(int n) {
    if (n <= 1) {
        throw std::runtime_error(fmt::format("n must be greater than one: {}", n));
    }

    return static_cast<std::size_t>(n);
}

AxbAlgorithm validate_algorithm(std::string algorithm_name) {
    if (algorithm_name == "pj") {
        return AxbAlgorithm::PointJacobi;
    }

    if (algorithm_name == "gs") {
        return AxbAlgorithm::GaussSeidel;
    }

    if (algorithm_name == "sor") {
        return AxbAlgorithm::SuccessiveOverRelaxation;
    }

    throw std::runtime_error(fmt::format(
        "Invalid algorithm name, must be pj/gs/sor: {}", algorithm_name
    ));
}

template<std::floating_point T>
T validate_relaxation_factor(T w) {
    if (w <= T{1}) {
        throw std::runtime_error(fmt::format("Relaxation factor must be greater than one: {}", w));
    }
    return w;
}


int main(int argc, char *argv[]) {
    const Header header {.description =
        "Solving Ax=b using iterative methods: PJ, GS, and SOR,\n"
        "where A is specially generated function"
    };

    argparse::ArgumentParser program{
        "generate_matrix",
        "1.0",
        argparse::default_arguments::help,
    };

    program.add_argument("n")
           .help("Matrix Rank")
           .scan<'i', int>();

    program.add_argument("-o", "--output")
           .help("Path to output file, if not provided the output is written to stdout");

    program.add_argument("-a", "--algorithm")
           .help("Algorithm to use")
           .default_value(std::string{ "sor" })
           .choices("pj", "gs", "sor");

    program.add_argument("--tol", "--tolerance")
           .help("Convergence tolerance")
           .default_value(real{1.0e-8})
           .scan<'g', real>();

    program.add_argument("--iter", "--max-iterations")
           .help("Maximum number of iterations")
           .default_value(10)
           .scan<'i', int>();

    program.add_argument("-w", "--relaxation-factor")
           .help("Relaxation factor for SOR (ignored otherwise)")
           .scan<'g', real>()
           .default_value(real{1.05});

    try {
        program.parse_args(argc, argv);

        const auto n = validate_matrix_rank(program.get<int>("n"));
        const auto algo = validate_algorithm(program.get<std::string>("-a"));
        const auto settings = FixedPointIterSettings<real>{
            program.get<real>("--tol")
          , program.get<int>("--iter")
        };
        const auto relaxation_factor = validate_relaxation_factor<real>(
            program.get<real>("-w")
        );

        const Outlab6<real> lab{
            .settings = settings,
            .linear_system = build_system<real>(n),
            .algorithm = algo,
            .relaxation_factor = relaxation_factor,
        };

        const auto result = lab.run();

        if (const auto output_filename = program.present<std::string>("--output");
            output_filename.has_value()
        ) {
            std::ofstream output{ output_filename.value() };
            if (!output.is_open()) {
                throw std::runtime_error(
                    fmt::format("Could not open: '{}'", output_filename.value())
                ); // Indicate an error occurred
            }
            header.echo(output);
            result.echo(output);
        }
        else {
            header.echo(std::cout);
            result.echo(std::cout);
        }
    }
    catch (const std::exception& err) {
        std::cerr << "\n"
                << format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
                << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}