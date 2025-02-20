#include <fstream>
#include <istream>
#include <ostream>
#include <filesystem>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>

#include <argparse/argparse.hpp>


#include "methods/linalg/Axb/solve.h"

template <std::floating_point DType>
struct Outlab6 {
    FixedPointIterSettings<DType> settings{};
    std::pair<Matrix<DType>, std::vector<DType>> linear_system;
    AxbAlgorithm algorithm;
    DType relaxation_factor{1.1};

    struct Result {
        const Outlab6& problem;
        IterativeAxbResult<DType> result{};

        auto echo(std::ostream& out) const -> void {
            problem.echo(out);
            fmt::println(out, "================================================================================");
            fmt::println(out, "{:^80s}", "Results");
            fmt::println(out, "--------------------------------------------------------------------------------");
            fmt::println(out, "{}", result.to_string());
            fmt::println(out, "Solution Vector, x:");
            fmt::println(out, "[{: 12.6e}]", fmt::join(result.x, " "));
            fmt::println(out, "================================================================================");
        }
    };

    auto echo(std::ostream& out) const -> void {
        fmt::println(out, "{:^80s}", "Inputs");
        fmt::println(out, "--------------------------------------------------------------------------------");
        fmt::println(out, "Selected method: {}", algorithm);
        fmt::println(out, "{:s}", settings.to_string());
        fmt::println(out, "--------------------------------------------------------------------------------");
        fmt::println(out, "Original matrix, A:");
        out << A() << "\n\n";
        fmt::println(out, "RHS vector, b:");
        fmt::println(out, "[{: 12.6e}]", fmt::join(b(), " "));
    }

    [[nodiscard]] constexpr
    auto run() const -> Result {
        if (not is_diag_nonzero(A())) {
            throw std::invalid_argument("Diagonal of A contains values close to zero");
        }

        IterativeAxbResult<DType> iter_result;

        switch (algorithm) {
            case AxbAlgorithm::PointJacobi: {
                iter_result = point_jacobi<DType>(linear_system, settings);
                break;
            }
            case AxbAlgorithm::GaussSeidel: {
                iter_result = gauss_seidel<DType>(linear_system, settings);
                break;
            }
            case AxbAlgorithm::SuccessiveOverRelaxation: {
                iter_result = successive_over_relaxation<DType>(linear_system, relaxation_factor, settings);
                break;
            }
            default:
                std::unreachable();
        }

        return {
            .problem = *this,
            .result = iter_result,
        };
    }

    [[nodiscard]]
    static auto from_file(std::istream& input) -> Outlab6 {
        const auto algorithm = read_axb_algorithm(input);

        DType relaxation_factor{1.1};
        switch (algorithm) {
            case AxbAlgorithm::SuccessiveOverRelaxation: {
                relaxation_factor = read_positive_value<DType>(input);
                if (relaxation_factor <= 1.0) {
                    throw std::runtime_error(fmt::format(
                        "SOR relaxation factor must be larger than 1: {}",
                        relaxation_factor
                    ));
                }
                break;
            }
            default: {
                break;
            }
        }

        return {
            .settings = FixedPointIterSettings<>::from_file(input),
            .linear_system = read_linear_system<DType>(input),
            .algorithm = algorithm,
            .relaxation_factor = relaxation_factor,
        };
    }

    [[nodiscard]] constexpr
    auto A() const -> Matrix<DType> {
        return linear_system.first;
    }

    [[nodiscard]] constexpr
    auto b() const -> std::span<const DType> {
        return std::span{linear_system.second};
    }
};


template <std::floating_point DType>
[[nodiscard]]
auto read_input_file(const std::string& filename) -> Outlab6<DType> {
    const auto input_filepath = std::filesystem::path{ filename };

    if (input_filepath.empty()) {
        throw std::runtime_error(
            fmt::format("Input file does not exist: {}", input_filepath.string())
        ); // Indicate an error occurred
    }

    std::ifstream input{ input_filepath };

    if (!input.is_open()) {
        throw std::runtime_error(
            fmt::format("Could not open input file: {}", input_filepath.string())
        ); // Indicate an error occurred
    }

    try {
        const auto lab = Outlab6<DType>::from_file(input);
        input.close();
        return lab;
    }
    catch (const std::exception&) {
        input.close();
        throw;
    }
}

struct Header {
    std::string title {"NE 591 Inlab #06"};
    std::string author {"Kirill Shumilov"};
    std::string date {"02/14/2025"};
    std::string description {
        "Solving Ax=b using iterative methods: PJ, GS, and SOR"
    };

    [[nodiscard]] constexpr auto to_string() const -> std::string {
        return fmt::format(
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
    }

    void echo(std::ostream& out) const {
        out << this->to_string();
    }
};

int main(int argc, char** argv) {
    const Header header{};

    argparse::ArgumentParser program{
        "shumilov_outlab6",
        "1.0",
        argparse::default_arguments::help,
    };

    program.add_argument("input")
           .help("Path to input file");

    program.add_argument("-o", "--output")
           .help("Path to output file");

    try {
        program.parse_args(argc, argv);
        const auto lab = read_input_file<long double>(program.get<std::string>("input"));

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
