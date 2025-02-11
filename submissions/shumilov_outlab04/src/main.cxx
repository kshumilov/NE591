#include <algorithm>  // min
#include <cassert>
#include <concepts>  // floating_point
#include <cstdlib>  // Exit Success
#include <format>
#include <fstream>
#include <iostream>  // cout
#include <optional>
#include <ostream>
#include <ranges>
#include <vector>
#include <utility>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ranges.h>

#include <argparse/argparse.hpp>

#include "linalg/matrix.h"
#include "linalg/lu.h"


template<class T>
auto read_value(std::istream& in) -> std::optional<T>
{
    T result{};

    if (in >> result) {
        return std::make_optional<T>(result);
    }

    return std::nullopt;
}


auto read_rank(std::istream& in) -> std::ptrdiff_t
{
    if (const auto result = read_value<std::ptrdiff_t>(in); result.has_value() and result.value() > 0) {
        return result.value();
    }

    throw std::runtime_error("Could not read `rank` from the input file.\n"
                             "Must be an integer larger than 1.");
}


template<std::floating_point scalar_t>
auto read_matrix_elements(std::istream& in, const std::size_t rows, const std::size_t cols, std::invocable<std::size_t, std::size_t> auto select)
{
    auto func = [&in, &select](const std::size_t row, const std::size_t col) -> scalar_t {
        if (select(row, col)) {
            if (const auto result = read_value<scalar_t>(in); result.has_value()) {
                return result.value();
            }

            throw std::runtime_error(
                std::format("Could not read matrix element ({:d}, {:d})\n"
                            "Must be a real value.", row + 1U, col + 1U)
            );
        }
        return scalar_t{};
    };

    return Matrix<scalar_t>::from_func(rows, cols, func);
}


template<std::floating_point scalar_t>
auto read_general_matrix(std::istream& in, const std::size_t rows, const std::size_t cols) -> Matrix<scalar_t>
{
    auto always_true = [](const auto, const auto) -> bool {
        return true;
    };

    return read_matrix_elements<scalar_t>(in, rows, cols, always_true);
}


template<class T>
auto read_vector(std::istream& is, const std::size_t size) -> std::vector<T>
{
    std::vector<T> values{};

    while (values.size() < size) {
        if (const auto result = read_value<T>(is); result.has_value()) {
            values.emplace_back(result.value());
        }
        else {
            throw std::runtime_error(
                std::format("Could not read vector element {:d}.", values.size())
            );
        }
    }

    return values;
}


template<std::floating_point scalar_t>
auto read_data_from_file(std::string filename) -> std::pair<Matrix<scalar_t>, std::vector<scalar_t>>
{
    std::ifstream in{filename};

    if (!in.is_open()) {
        in.close();
        throw std::runtime_error(
            std::format("Could not open '{}'", filename)); // Indicate an error occurred
    }

    const auto rank{read_rank(in)};
    const auto A = read_general_matrix<scalar_t>(in, rank, rank);
    const auto b = read_vector<scalar_t>(in, rank);
    return std::make_pair(A, b);
}


void outlab04(const Matrix<double>& A, const std::vector<double>& b)
{

    fmt::print(
        "================================================================================\n"
        "NE 591 Outlab #04: Solution of Ax=b using LU Factorization\n"
        "Author: Kirill Shumilov\n"
        "Date: 01/31/2025\n"
        "================================================================================\n");

    fmt::println("{:^80s}", "Inputs");
    fmt::println("--------------------------------------------------------------------------------");
    fmt::println("Original Matrix, A(rank = {:d}):", A.rows());
    std::cout << A.to_string() << '\n';
    fmt::println("\nRHS vector, b:");
    fmt::println("[{:12.6f}]", fmt::join(b, " "));

    const auto& [L, U] = lu(A);
    const auto Ap = L * U;
    const auto R = A - Ap;
    const auto norm = R.norm();

    fmt::println("================================================================================");
    fmt::println("{:^80s}", "Results");
    fmt::println("--------------------------------------------------------------------------------");
    fmt::println("Lower Triangular Matrix, L:");
    std::cout << L.to_string() << std::endl;
    fmt::println("\nUpper Triangular Matrix, L:");
    std::cout << U.to_string() << std::endl;
    fmt::println("\nMatrix Residual, R = LU - A, with norm |R| = {: 12.6e}", norm);
    std::cout << R.to_string() << '\n';

    const auto x = solve_lu<double, true>(L, U, b);
    const auto bp = A * x;
    std::vector<double> residual {};
    std::transform(
        b.cbegin(), b.cend(), bp.cbegin(), std::back_inserter(residual),
        std::minus<double>{}
    );

    const auto max_residual = std::transform_reduce(
        residual.cbegin(), residual.cend(),
        double{},
        [](const auto a, const auto b) {
          return a > b ? a : b;
        },
        [](const auto a) {
          return std::abs(a);
        }
    );

    fmt::println("--------------------------------------------------------------------------------");
    fmt::println("Solution vector, x:");
    fmt::println("[{:12.6f}]", fmt::join(x, " "));

    fmt::println("\nResidual vector, r = b - Ax, max|r| = {: 12.6f}", max_residual);
    fmt::println("[{:12.6f}]", fmt::join(residual, " "));
    fmt::println("================================================================================");
}


int main(int argc, char *argv[])
{

    std::string description =
        "This program solves system of equations Ax=LUx=b\n"
        "by performing LU factorization of A and then\n"
        "using forward and backward substitution based on\n"
        "lower triangular (L), upper-triangular (U), and RHS vector (b)";

    argparse::ArgumentParser program{
        "shumilov_outlab04",
        "1.0",
    };

    program.add_description(description);

    program.add_argument("filename")
               .help("Path to input fle");

    try {
        program.parse_args(argc, argv);

        const auto& [A, b] = read_data_from_file<double>(program.get<std::string>("filename"));

        outlab04(A, b);
    }
    catch (const std::exception& err) {
        std::cerr << "\n"
          << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
          << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
