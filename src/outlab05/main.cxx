#include <ostream>
#include <iostream>
#include <concepts>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>  // ifstream
#include <utility>

// 3rd-party Dependencies
#include <fmt/core.h>
#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/ostream.h>
#include <argparse/argparse.hpp>

// Local Implementations
#include "methods/linalg/matrix.h"
#include "methods/linalg/lu.h"
#include "methods/linalg/utils/io.h"
#include "methods/linalg/blas.h"

#include "methods/utils/io.h"
#include "methods/array.h"


template <std::floating_point scalar_t>
struct Outlab5 {
    Matrix<scalar_t> A{};
    std::vector<scalar_t> b {};
    PivotingMethod pivoting_method {PivotingMethod::PartialPivoting};

    struct Result {
        const Outlab5* problem {};
        Matrix<scalar_t> L {};
        Matrix<scalar_t> U {};
        std::optional<Matrix<scalar_t>> P {std::nullopt};
        std::vector<scalar_t> x{};

        auto echo(std::ostream& out) const -> void
        {
            fmt::println(out, "{:^80s}", "Results");
            fmt::println(out, "--------------------------------------------------------------------------------");
            fmt::println(out, "Lower Triangular Matrix, L{}:", L.shape_info());
            out << L << "\n\n";

            fmt::println(out, "Upper Triangular Matrix, U{}:", U.shape_info());
            out << U << "\n\n";

            if (P.has_value()) {
                fmt::println(out, "Permutation Matrix, P{}:", P.value().shape_info());
                out << P.value() << "\n\n";
            }

            const auto R = lu_residual();
            fmt::println("Matrix Residual, R{} = L * U - P * A,\n"
                         "with norm |R| = {: 12.6e} and max|Rij| = {: 12.6e}:",
                         R.shape_info(), R.norm(), max_abs(R.data()));
            out << R << "\n";

            fmt::println(out, "--------------------------------------------------------------------------------");
            fmt::println(out, "Solution Vector, x:");
            fmt::println(out, "[{: 12.6e}]\n", fmt::join(x, " "));

            const auto r = b_residual();
            fmt::println(out, "Solution Residual, r = b - A * x,\n"
                              "with norm |r| = {: 12.6e} and max|ri| = {: 12.6e}:",
                              norm_l2(r), max_abs(r));
            fmt::println(out, "[{: 12.6e}]", fmt::join(r, " "));
        }

        [[nodiscard]] constexpr
        auto lu_residual() const -> Matrix<scalar_t>
        {
            auto R{P.has_value() ? P.value() * problem->A : problem->A};

            // R = L * U - P * A
            gemm<scalar_t>(L, U, R, scalar_t{1.0}, scalar_t{-1.0});
            return R;
        }

        [[nodiscard]] constexpr
        auto b_residual() const -> std::vector<scalar_t>
        {
            auto res{problem->b};
            // res = b - A * x;
            gemv<scalar_t>(problem->A, x, res, scalar_t{-1}, scalar_t{1});
            return res;
        }
    };

    auto echo(std::ostream& out) const -> void
    {
        fmt::println(out, "{:^80s}", "Inputs");
        fmt::println(out, "--------------------------------------------------------------------------------");
        fmt::println(out, "Original Matrix, A{}:", A.shape_info());
        out << A << "\n\n";

        fmt::println(out, "RHS vector, b:");
        fmt::println(out, "[{: 12.6e}]\n", fmt::join(b, " "));

        fmt::println(out, "Pivoting Method: {}", pivoting_method);
    }

    [[nodiscard]]
    auto solve() const -> Result
    {
        if (pivoting_method == PivotingMethod::NoPivoting) {
            const auto& [L, U, result] = lu_factor(A);
            const auto& x = lu_solve<scalar_t>(L, U, b);

            if (result == LUResult::SmallPivotEncountered) {
                std::cerr << fmt::format(
                    fmt::emphasis::bold | fg(fmt::color::red),
                    "Error: Small Pivot Encountered"
                ) << std::endl;
            }

            return {
                .problem = this,
                .L = L,
                .U = U,
                .P = std::nullopt,
                .x = x
            };
        }

        const auto& [L, U, P, result] = lup_factor(A);
        if (result == LUResult::SmallPivotEncountered) {
            std::cerr << fmt::format(
                fmt::emphasis::bold | fg(fmt::color::red),
                "Error: Small Pivot Encountered"
            ) << std::endl;
        }

        const auto& x = lup_solve<scalar_t>(L, U, P, b);

        return {
            .problem = this,
            .L = L,
            .U = U,
            .P = std::make_optional(P),
            .x = x
        };

    }

    [[maybe_unused]]
    auto run() const -> Result
    {
        echo(std::cout);
        fmt::println("================================================================================");

        const auto result = solve();
        result.echo(std::cout);
        fmt::println("================================================================================");

        return result;
    }

    [[nodiscard]]
    static auto from_file(std::string_view filename) -> Outlab5
    {
        std::ifstream in{filename.data()};

        if (!in.is_open()) {
            in.close();
            throw std::runtime_error(
                fmt::format("Could not open '{}'", filename)
            ); // Indicate an error occurred
        }

        try {
            const auto rank = read_rank(in);

            const Outlab5 problem {
                .A = read_matrix<scalar_t, MatrixSymmetry::General>(in, rank, rank),
                .b = read_vector<scalar_t>(in, rank),
                .pivoting_method = read_pivoting_method(in)
            };

            in.close();

            return problem;
        }
        catch ([[maybe_unused]] const std::exception& err) {
            in.close();
            throw;
        }
    }
};



int main(int argc, char* argv[])
{
    const std::string author {"Kirill Shumilov"};
    const std::string description {
        "This program solves system of equations Ax=b\n"
        "using LUP Factorization, PAx=LUx=Pb,\n"
        "using forward and backward substitution based on\n"
        "lower triangular matrix (L), upper-triangular matrix (U),\n"
        "row permutation matrix (P), and RHS vector (b)\n"
    };

    fmt::print(
        "================================================================================\n"
        "NE 591 Outlab #05: Solution of Ax=b using LUP Factorization Result\n"
        "Author: {:s}\n"
        "Date: 02/07/2025\n"
        "--------------------------------------------------------------------------------\n"
        "{:s}"
        "================================================================================\n",
        author, description
    );

    argparse::ArgumentParser program{
        "shumilov_inlab05",
        "1.0",
        argparse::default_arguments::help
    };

    program.add_description(description);

    program.add_argument("filename")
               .help("Path to input file");

    try {
        program.parse_args(argc, argv);
        const auto filename = program.get<std::string>("filename");
        const auto problem = Outlab5<long double>::from_file(filename);
        problem.run();
    }
    catch (const std::exception& err) {
        std::cerr << "\n"
          << format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
          << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
