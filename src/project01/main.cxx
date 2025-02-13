#include <ostream>
#include <iostream>
#include <concepts>
#include <string>
#include <string_view>
#include <fstream>  // ifstream

// 3rd-party Dependencies
#include <fmt/core.h>
#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/ostream.h>
#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>

// Local Implementations
// LinAlg and LU
#include "methods/linalg/matrix.h"
#include "methods/linalg/lu.h"
#include "methods/linalg/utils/io.h"

// IO and Array manipulation
#include "methods/utils/io.h"
#include "methods/array.h"


using json = nlohmann::json;


template <std::floating_point scalar_t>
struct Project01 {
    scalar_t a{};
    scalar_t b{};
    std::ptrdiff_t M{};
    std::ptrdiff_t N{};
    scalar_t D{};
    scalar_t Sa{};
    const Matrix<scalar_t> source{};

    struct Result {
        const Project01* problem{};
        const Matrix<scalar_t> flux{};

        [[nodiscard]]
        constexpr auto to_string() const -> std::string
        {
            std::string result{};
            result.append("================================================================================\n");
            result.append(problem->to_string());
            result.append("================================================================================\n");
            fmt::format_to(std::back_inserter(result), "{:^80s}\n", "Results");
            result.append("--------------------------------------------------------------------------------\n");
            fmt::format_to(std::back_inserter(result), "Flux Matrix, psi{}:\n", flux.shape_info());
            result.append(flux.to_string());
            result.push_back('\n');
            result.append("================================================================================\n");
            return result;
        }

        auto echo(std::ostream& out) const -> void
        {
           out << to_string() << std::endl;
        }

        template<class BasicJsonType>
        friend void to_json(BasicJsonType& j, const Result& result)
        {
            j["flux"] = result.flux;
            j["problem"] = *result.problem;
        }
    };

    [[nodiscard]]
    auto solve() const -> Result
    {
        auto A = setup_operator();

        #ifndef NDEBUG
        fmt::println("Operator, A{}", A.shape_info());
        std::cout << A.to_string() << std::endl;

        for (const auto r : std::views::iota(0U, A.rows())) {
            const auto begin = A.data().cbegin() + r * A.cols();
            const auto end = A.data().cbegin() + (r + 1) * A.cols();
            const auto n = std::count_if(
                begin, end,
                [](const auto x) { return x != scalar_t{0.0}; }
            );
            fmt::println("Row {} has {} non-zero value", r, n);
        }
        #endif

        const auto& [P, result] = lup_factor_inplace<scalar_t>(A);

        if (result == LUResult::SmallPivotEncountered) {
            std::cerr << fmt::format(
                fmt::emphasis::bold | fg(fmt::color::red),
                "Error: Small Pivot Encountered"
            ) << std::endl;
        }

        // const Matrix<scalar_t> psi{dim, dim, lup_solve<scalar_t>(A, P, q.data())};
        return Result{
            .problem = this,
            .flux{
                static_cast<std::size_t>(M),
                static_cast<std::size_t>(N),
                lup_solve<scalar_t>(A, P, source.data())
            }
        };

    }

    [[nodiscard]] constexpr
    auto setup_operator() const -> Matrix<scalar_t>
    {
        const auto M_ = static_cast<std::size_t>(M);
        const auto delta = step(0.0, a, M + 1);
        const auto D_over_delta_sq = D / (delta * delta);

        const auto N_ = static_cast<std::size_t>(N);
        const auto gamma = step(0.0, b, N + 1);
        const auto D_over_gamma_sq = D / (gamma * gamma);

        const auto dim = operator_dimension();
        return Matrix<scalar_t>::from_func(
            dim, dim,
            [&](const auto I, const auto J) -> scalar_t {
                const auto [i, j] = pair_from_flat_idx(I, N_);
                const auto [k, l] = pair_from_flat_idx(J, N_);

                if (((0U < i && i - 1U == k) || (i < M_ - 1U && i + 1U == k)) && j == l) return -D_over_delta_sq;
                if (i == k && ((0U < j && j - 1U == l) || (j < N_ - 1U && j + 1U == l))) return -D_over_gamma_sq;
                if (i == k && j == l) return 2.0 * (D_over_delta_sq + D_over_gamma_sq) + Sa;

                return scalar_t{0.0};
            }
        );
    }

    [[nodiscard]] constexpr
    auto operator_dimension() const -> std::size_t
    {
        return static_cast<std::size_t>(N * M);
    }

    [[nodiscard]]
    constexpr auto to_string() const -> std::string
    {
        std::string result{};
        fmt::format_to(std::back_inserter(result), "{:^80s}\n", "Inputs");
        fmt::format_to(std::back_inserter(result), "--------------------------------------------------------------------------------\n");
        fmt::format_to(std::back_inserter(result), "Problem Dimensions: {: 12.6e} x {: 12.6e}\n", a, b);
        fmt::format_to(std::back_inserter(result), "Grid Dimensions: {:d} x {:d}\n", M + 2U, N + 2U);
        fmt::format_to(std::back_inserter(result), "Diffusion Coefficient, D: {: 12.6e}\n", D);
        fmt::format_to(std::back_inserter(result), "Macroscopic Removal Cross Section, Sa: {: 12.6e}\n", Sa);
        fmt::format_to(std::back_inserter(result), "Source Matrix, q{}:\n", source.shape_info());
        result.append(source.to_string());
        result.push_back('\n');
        return result;
    }

    auto echo(std::ostream& out) const -> void
    {
       out << to_string();
    }

    [[nodiscard]]
    static auto read_file(std::istream& input) -> Project01
    {
        [[maybe_unused]] const auto flag = read_value<int>(input);

        const auto a = read_positive_value<scalar_t>(input, "a"sv);
        const auto b = read_positive_value<scalar_t>(input, "b"sv);
        const auto M = read_rank(input, "M");
        const auto N = read_rank(input, "N");

        return {
            .a = a, .b = b, .M = M, .N = N,
            .D = read_nonnegative_value<scalar_t>(input, "D"),
            .Sa = read_nonnegative_value<scalar_t>(input, "Sa"),
            .source = read_matrix<scalar_t, MatrixSymmetry::General>(input, M, N),
        };
    }

    [[nodiscard]]
    static auto from_file(std::string_view filename) -> Project01
    {
        std::ifstream in{filename.data()};

        if (!in.is_open()) {
            in.close();
            throw std::runtime_error(
                fmt::format("Could not open '{}'", filename)
            ); // Indicate an error occurred
        }

        try {
            const auto problem = Project01::read_file(in);
            in.close();
            return problem;
        }
        catch ([[maybe_unused]] const std::exception& err) {
            in.close();
            throw;
        }
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Project01, a, b, M, N, D, Sa, source)
};


int main(const int argc, char* argv[])
{
    const std::string title {"NE 591 Project #01\n"};
    const std::string author {"Kirill Shumilov"};
    const std::string date {"02/14/2025"};
    const std::string description {
        "Solving 2D steady state, one speed diffusion equation in a non-multiplying,\n"
        "isotropic scattering homogeneous medium, using LUP factorization"
    };

    fmt::print(
        "================================================================================\n"
        "{:s}\n"
        "Author: {:s}\n"
        "Date: {:s}\n"
        "--------------------------------------------------------------------------------\n"
        "{:s}\n"
        "================================================================================\n",
        title, author, date, description
    );

    argparse::ArgumentParser program{
        "shumilov_project01",
        "1.0",
    };

    program.add_description(description);

    program.add_argument("filename")
               .help("Path to input file");

    program.add_argument("--output-json")
        .help("Name of the output json file");

    try {
        program.parse_args(argc, argv);
        const auto input_filename = program.get<std::string>("filename");
        const auto problem = Project01<double>::from_file(input_filename);
        const auto result = problem.solve();

        result.echo(std::cout);

        if (const auto output_filename = program.present<std::string>("--output-json");
            output_filename.has_value()) {
            const json j = result;
            std::ofstream o(output_filename.value());
            o << std::setw(4) << j << std::endl;
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
