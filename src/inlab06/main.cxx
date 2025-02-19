#include <fstream>

#include <fmt/core.h>
#include <fmt/color.h>

#include <argparse/argparse.hpp>

#include "methods/linalg/utils/io.h"
#include "methods/linalg/pj.h"


int main(int argc, char** argv) {
    const std::string title {"NE 591 Inlab #06"};
    const std::string author {"Kirill Shumilov"};
    const std::string date {"02/14/2025"};
    const std::string description {
        "Solving Ax=b using iterative methods: PJ, GS, and SOR"
    };

    argparse::ArgumentParser program{
        "shumilov_project01",
        "1.0",
        argparse::default_arguments::help,
    };

    program.add_argument("filename")
               .help("Path to input file");

    try {
        fmt::print(
            "================================================================================\n"
            "{:s}\n"
            "Author: {:s}\n"
            "Date: {:s}\n"
            "--------------------------------------------------------------------------------\n"
            "{:s}\n"
            "================================================================================\n"
          , title
          , author
          , date
          , description
        );

        program.parse_args(argc, argv);
        const auto input_filename = program.get<std::string>("filename");

        std::ifstream in{input_filename};

        if (!in.is_open()) {
            throw std::runtime_error(
                fmt::format("Could not open '{}'", input_filename)
            ); // Indicate an error occurred
        }

        const auto algo = read_nonnegative_value<int>(in, "Algorithm");
        if (algo < 0 || algo > 2) {
            in.close();
            throw std::runtime_error(fmt::format("Invalid algorithm code, must be 0/1/2: {}", algo));
        }

        if (algo != 0) {
            in.close();
            throw std::runtime_error(fmt::format("GS and SOR algorithms are not supported"));
        }

        const auto rank = read_positive_value<int>(in);
        const auto tol = read_nonnegative_value<long double>(in, "tolerance");
        const auto max_iter = read_nonnegative_value<int>(in, "max_iter");

        auto A = read_matrix<double>(in, rank, rank);
        const auto b = read_vector<double>(in, rank);

        const auto result = point_jacobi<double>(A, b, {max_iter, tol});

        fmt::println("{:^80s}", "Inputs");
        fmt::println("--------------------------------------------------------------------------------");
        fmt::println("Selected method: Point-Jacobi");
        fmt::println("Tolerance: {}", tol);
        fmt::println("Max Iterations: {}", max_iter);
        fmt::println("--------------------------------------------------------------------------------");
        fmt::println("Original matrix, A:");
        std::cout << A << "\n\n";
        fmt::println("RHS vector, b:");
        fmt::println("[{: 12.6e}]", fmt::join(b, " "));
        fmt::println("================================================================================");
        fmt::println("{:^80s}", "Results");
        fmt::println("--------------------------------------------------------------------------------");
        fmt::println("{}", result.to_string());
        fmt::println("Solution Vector, b:");
        fmt::println("[{: 12.6e}]", fmt::join(result.x, " "));
        fmt::println("================================================================================");
    }
    catch (const std::exception& err) {
        std::cerr << "\n"
          << format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
          << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;

}
