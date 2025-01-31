#include <iostream>
#include <istream>
#include <cstdlib>
#include <cstddef>
#include <vector>
#include <span>

#include <fmt/ranges.h>
#include <fmt/format.h>
#include <fmt/color.h>

#include <linalg/matrix.h>
#include <linalg/lu.h>

using idx_t = std::size_t;
using namespace std::literals;


auto read_matrix_rank(std::istream& is, std::string_view propmpt = ""sv) -> idx_t
{
    if (not propmpt.empty()) {
        fmt::println("{:s}", propmpt);
    }

    std::size_t rank {0};
    is >> rank;
    return rank;
}


template<std::floating_point scalar_t>
auto read_lt_matrix(std::istream& is, const std::size_t rows, const std::size_t cols, std::string_view prompt = ""sv) -> Matrix<scalar_t>
{
    if (not prompt.empty()) {
        fmt::println("{:s}", prompt);
    }

    auto func = [&](const std::size_t row, const std::size_t col) -> scalar_t {
        scalar_t value {};
        if (row >= col) {
            is >> value;
            return value;
        }
        return scalar_t{0.0};
    };

    return Matrix<scalar_t>::from_func(rows, cols, func);
}


template<std::floating_point scalar_t>
auto read_vector(std::istream&, const std::size_t size, std::string_view prompt = ""sv) -> std::vector<scalar_t>
{
    if (not prompt.empty()) {
        fmt::println("{:s}", prompt);
    }

    std::vector<scalar_t> values{};

    while (values.size() < size) {
        scalar_t value {};
        std::cin >> value;
        values.emplace_back(value);
    }

    return values;
}


template<std::floating_point scalar_t>
auto read_ut_matrix(std::istream& is, const std::size_t rows, const std::size_t cols, std::string_view prompt = ""sv) -> Matrix<scalar_t>
{
    if (not prompt.empty()) {
        fmt::println("{:s}", prompt);
    }

    auto func = [&](const std::size_t row, const std::size_t col) -> scalar_t {
        scalar_t value {};
        if (row <= col) {
            is >> value;
            return value;
        }
        return scalar_t{0.0};
    };

    return Matrix<scalar_t>::from_func(rows, cols, func);
}


int main()
{
    fmt::print(
        "================================================================================\n"
        "NE 591 Inlab #04: Solution State of LU Factorization\n"
        "Author: Kirill Shumilov\n"
        "Date: 01/31/2025\n"
        "================================================================================\n"
        "This program performs solves system of equations Ax=LUx=b\n"
        "using forward and backward substitution based on\n"
        "lower triangular (L), upper-triangular (U), and RHS vector (b)\n"
        "================================================================================\n"
    );

    try {
        const idx_t rows = read_matrix_rank(std::cin, "Enter matrix rank:");
        const auto L = read_lt_matrix<double>(std::cin, rows, rows,
                                              "Enter non-zero values of lower triangular matrix in row-major order:");
        const auto U = read_ut_matrix<double>(std::cin, rows, rows,
                                              "Enter non-zero values of upper triangular matrix in row-major order:");
        const auto b = read_vector<double>(std::cin, rows, "Enter RHS vector:");

        fmt::println("================================================================================");
        fmt::println("{:^80s}", "Inputs");
        fmt::println("--------------------------------------------------------------------------------");
        L.display("Lower triangular matrix: ");
        fmt::println("--------------------------------------------------------------------------------");
        U.display("Upper triangular matrix: ");
        fmt::println("--------------------------------------------------------------------------------");

        const auto A = L * U;
        A.display("Original Matrix: ", "A = L * U");

        fmt::println("--------------------------------------------------------------------------------");
        fmt::println("RHS Vector, b:");
        fmt::println("{:}", b);

        const auto x = solve_lu<double>(L, U, b);

        fmt::println("================================================================================");
        fmt::println("{:^80s}", "Results");
        fmt::println("--------------------------------------------------------------------------------");
        fmt::println("Solution vector, x:");
        fmt::println("{:}", x);
        fmt::println("================================================================================");
    }
    catch (const std::exception& err) {
        std::cerr << "\n"
                << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
                << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;

}