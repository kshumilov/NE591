#ifndef LAB06_H
#define LAB06_H

#include <concepts>
#include <utility>

#include "methods/linalg/Axb/solve.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

template<std::floating_point DType>
struct Outlab6
{
    FixedPointIterSettings<DType> settings{};
    std::pair<Matrix<DType>, std::vector<DType>> linear_system;
    AxbAlgorithm algorithm{};
    DType relaxation_factor{ 1.1 };

    struct Result
    {
        const Outlab6& problem;
        IterativeAxbResult<DType> result{};


        auto echo(std::ostream& out) const -> void
        {
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


    auto echo(std::ostream& out) const -> void
    {
        fmt::println(out, "{:^80s}", "Inputs");
        fmt::println(out, "--------------------------------------------------------------------------------");
        fmt::println(out, "Matrix Rank: {:d}", A().rows());
        fmt::println(out, "Selected method: {}", algorithm);
        fmt::println(out, "{:s}", settings.to_string());
        if (algorithm == AxbAlgorithm::SuccessiveOverRelaxation)
        {
            fmt::println(out, "\tRelaxation Factor: {:12.6e}", relaxation_factor);
        }

        if (A().rows() <= 10)
        {
            fmt::println(out, "--------------------------------------------------------------------------------");
            fmt::println(out, "Original matrix, A:");
            out << A() << "\n\n";
            fmt::println(out, "RHS vector, b:");
            fmt::println(out, "[{: 12.6e}]", fmt::join(b(), " "));
        }
    }


    [[nodiscard]] constexpr auto run() const -> Result
    {
        if (not is_diag_nonzero(A()))
        {
            throw std::invalid_argument("Diagonal of A contains values close to zero");
        }

        IterativeAxbResult<DType> iter_result;

        switch (algorithm)
        {
            case AxbAlgorithm::PointJacobi:
            {
                iter_result = point_jacobi<DType>(linear_system, settings);
                break;
            }
            case AxbAlgorithm::GaussSeidel:
            {
                iter_result = gauss_seidel<DType>(linear_system, settings);
                break;
            }
            case AxbAlgorithm::SuccessiveOverRelaxation:
            {
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
    static auto from_file(std::istream& input) -> Outlab6
    {
        const auto algorithm = read_axb_algorithm(input);

        DType relaxation_factor{ 1.1 };
        switch (algorithm)
        {
            case AxbAlgorithm::SuccessiveOverRelaxation:
            {
                relaxation_factor = read_positive_value<DType>(input);
                if (relaxation_factor <= 1.0)
                {
                    throw std::runtime_error(
                        fmt::format("SOR relaxation factor must be larger than 1: {}", relaxation_factor)
                    );
                }
                break;
            }
            default:
            {
                break;
            }
        }

        return {
            .settings = FixedPointIterSettings<DType>::from_file(input),
            .linear_system = read_linear_system<DType>(input),
            .algorithm = algorithm,
            .relaxation_factor = relaxation_factor,
        };
    }


    [[nodiscard]] constexpr auto A() const -> Matrix<DType> { return linear_system.first; }

    [[nodiscard]] constexpr auto b() const -> std::span<const DType> { return std::span{ linear_system.second }; }
};


struct Header
{
    std::string title{ "NE 591 Inlab #06" };
    std::string author{ "Kirill Shumilov" };
    std::string date{ "02/14/2025" };
    std::string description{ "Solving Ax=b using iterative methods: PJ, GS, and SOR" };


    [[nodiscard]] constexpr auto to_string() const -> std::string
    {
        return fmt::format(
            "================================================================================\n"
            "{:s}\n"
            "Author: {:s}\n"
            "Date: {:s}\n"
            "--------------------------------------------------------------------------------\n"
            "{:s}\n"
            "================================================================================\n",
            title,
            author,
            date,
            description
        );
    }


    void echo(std::ostream& out) const { out << this->to_string(); }
};
#endif // LAB06_H
