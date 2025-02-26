#include <concepts>
#include <fstream> // ifstream
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

// 3rd-party Dependencies
#include <argparse/argparse.hpp>
#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ostream.h>

// Local Implementations
#include "methods/linalg/lu.h"
#include "methods/linalg/matrix.h"
#include "methods/linalg/utils/io.h"


template<std::floating_point scalar_t> struct Lab05 {
    Matrix<scalar_t> L{};
    Matrix<scalar_t> U{};
    Matrix<scalar_t> P{};
    std::vector<scalar_t> b{};

    struct Result {
        const Lab05 *problem{};
        std::vector<scalar_t> x{};

        auto echo(std::ostream &out) const -> void {
            fmt::println(out, "{:^80s}", "Results");
            fmt::println(out, "--------------------------------------------------------------------------------");
            fmt::println(out, "Solution Vector, x:");
            fmt::println(out, "[{: 12.6e}]", fmt::join(x, " "));
        }
    };

    auto echo(std::ostream &out) const -> void {
        fmt::println(out, "{:^80s}", "Inputs");
        fmt::println(out, "--------------------------------------------------------------------------------");
        fmt::println(out, "Lower Triangular Matrix, L{}:", L.shape_info());
        out << L << "\n\n";

        fmt::println(out, "Upper Triangular Matrix, U{}:", U.shape_info());
        out << U << "\n\n";

        fmt::println(out, "Permutation Matrix, P{}:", P.shape_info());
        out << P << "\n\n";

        fmt::println(out, "RHS vector, b:");
        fmt::println(out, "[{: 12.6e}]", fmt::join(b, " "));
    }

    [[nodiscard]] constexpr auto solve() const -> Result {
        return {.problem = this, .x = lup_solve<scalar_t>(L, U, P, b)};
    }

    auto run() const -> Result {
        echo(std::cout);
        fmt::println("================================================================================");

        const auto result = solve();
        result.echo(std::cout);
        fmt::println("================================================================================");

        return result;
    }

    [[nodiscard]]
    static auto from_file(std::string filename) -> Lab05 {
        std::ifstream in{filename};

        if (!in.is_open()) {
            in.close();
            throw std::runtime_error(fmt::format("Could not open '{}'", filename)); // Indicate an error occurred
        }

        try {
            const auto rank = read_rank(in);

            const Lab05 result{
                    .L = read_matrix<scalar_t, MatrixSymmetry::General>(in, rank, rank),
                    .U = read_matrix<scalar_t, MatrixSymmetry::General>(in, rank, rank),
                    .P = read_matrix<scalar_t, MatrixSymmetry::General>(in, rank, rank),
                    .b = read_vector<scalar_t>(in, rank)};

            in.close();

            return result;
        }
        catch ([[maybe_unused]] const std::exception &err) {
            in.close();
            throw;
        }
    }
};


int main(int argc, char *argv[]) {
    const std::string author{"Kirill Shumilov"};
    const std::string description{"This program solves system of equations Ax=b\n"
                                  "using LUP Factorization, PAx=LUx=Pb,"
                                  "using forward and backward substitution based on\n"
                                  "lower triangular matrix (L), upper-triangular matrix (U),\n"
                                  "row permutation matrix (P), and RHS vector (b)\n"};

    fmt::print(
            "================================================================================\n"
            "NE 591 Inlab #05: Solution of Ax=b using LUP Factorization Result\n"
            "Author: {:s}\n"
            "Date: 02/07/2025\n"
            "================================================================================\n"
            "{:s}"
            "================================================================================\n",
            author,
            description);

    argparse::ArgumentParser program{
            "shumilov_inlab05",
            "1.0",
    };

    program.add_description(description);

    program.add_argument("filename").help("Path to input file");

    try {
        program.parse_args(argc, argv);
        const auto filename = program.get<std::string>("filename");
        const auto problem = Lab05<double>::from_file(filename);
        problem.run();
    }
    catch (const std::exception &err) {
        std::cerr << "\n" << format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ") << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
