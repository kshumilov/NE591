#ifndef PROJECT02_H
#define PROJECT02_H

#include <string>
#include <concepts>

#include <fmt/core.h>

#include "methods/linalg/Axb/solve.h"
#include "project/diffusion_problem.h"
#include "project/diffusion_solver.h"


struct Header
{
    std::string title{ "NE 591 Project #01" };
    std::string author{ "Kirill Shumilov" };
    std::string date{ "02/28/2025" };
    std::string description{
        "Solving 2D steady state, one speed diffusion equation in a non-multiplying,\n"
        "isotropic scattering homogeneous medium, using LUP, PJ, GS, or SOR"
    };


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


template<std::floating_point T>
struct Parameters
{
    AxbAlgorithm algorithm{};
    FixedPointIterSettings<T> iter_settings{};
    T relaxation_factor{};

    template<class BasicJsonType>
    friend void to_json(BasicJsonType& j, const Parameters& params)
    {
        switch (params.algorithm)
        {
            case AxbAlgorithm::LUP:
            {
                j["algorithm"] = "lup";
                break;
            }
            case AxbAlgorithm::PointJacobi:
            {
                j["algorithm"] = "pj";
                break;
            }
            case AxbAlgorithm::GaussSeidel:
            {
                j["algorithm"] = "gs";
                break;
            }
            case AxbAlgorithm::SuccessiveOverRelaxation:
            {
                j["algorithm"] = "sor";
                break;
            }
            default:
                throw std::invalid_argument("Invalid algorithm");
        }

        if (params.algorithm != AxbAlgorithm::LUP)
        {
            j["iter_settings"] = params.iter_settings;
            j["relaxation_factor"] = params.relaxation_factor;
        }
    }

    template<class BasicJsonType>
    friend void from_json(BasicJsonType& j, Parameters& params)
    {
        std::string algorithm{};
        j.at("algorithm").template get_to<std::string>(algorithm);

        if (algorithm == "lup")
        {
            params.algorithm = AxbAlgorithm::LUP;
        }
        else if (algorithm == "pj")
        {
            params.algorithm = AxbAlgorithm::PointJacobi;
        }
        else if (algorithm == "gs")
        {
            params.algorithm = AxbAlgorithm::GaussSeidel;
        }
        else if (algorithm == "sor")
        {
            params.algorithm = AxbAlgorithm::SuccessiveOverRelaxation;
        }
        else
        {
            throw std::invalid_argument("Invalid algorithm");
        }

        if (params.algorithm != AxbAlgorithm::LUP)
        {
            params.iter_settings = j["iter_settings"].template get<FixedPointIterSettings<T>>();
            params.relaxation_factor = j["relaxation_factor"].template get<T>();
        }
    }
};


template<std::floating_point T>
struct Project02
{
    Parameters<T> params{};
    IsotropicSteadyStateDiffusion2D<T> problem{};

    struct Solution
    {
        const Project02& project;
        const Matrix<T> scalar_flux;
        const T residual_error{};
        std::chrono::duration<long long, std::nano> time{}; // nanoseconds

        const bool converged{};
        const T relative_error{};
        const int iters{};

        auto echo(std::ostream& out) const -> void
        {
            project.echo(out);
            fmt::print(
                out,
                "================================================================================\n"
                "{:^80s}\n"
                "--------------------------------------------------------------------------------\n"
                "Flux, phi {}:\n"
                "{}\n"
                "................................................................................\n"
                "Max abs residual: {:12.6e}\n",
                "Results", scalar_flux.shape_info(), scalar_flux.to_string(), residual_error
            );

            if (project.params.algorithm != AxbAlgorithm::LUP)
            {
                fmt::print(
                    out,
                    "{:s} #{: <5d}:\n"
                    "\tRelative error: {:12.6e}\n",
                    converged ? "Converged at iteration" : "Failed to converge in ",
                    iters,
                    relative_error
                );
            }

            fmt::print(
                out,
                "................................................................................\n"
                "Execution time: {:%S} seconds.\n"
                "================================================================================\n",
                time
            );
        }


        template<class BasicJsonType>
        friend void to_json(BasicJsonType& j, const Solution& solution)
        {
            j["project"] = solution.project;
            j["flux"] = solution.scalar_flux;
            j["time"] = solution.time.count();
            j["residual_error"] = solution.residual_error;
            j["relative_error"] = solution.relative_error;
            j["iterations"] = solution.iters;
            j["converged"] = solution.converged;
        }
    };

    auto echo(std::ostream& out) const -> void
    {
        fmt::print(
            out,
            "{:^80s}\n"
            "{:s}\n",
            "Inputs", problem.to_string()
        );

        fmt::print(
            out,
            "................................................................................\n"
            "Selected Method: {}\n",
            params.algorithm
        );

        if (params.algorithm != AxbAlgorithm::LUP)
        {
            fmt::print(
                out,
                "{:}\n",
                params.iter_settings.to_string()
            );

            if (params.algorithm == AxbAlgorithm::SuccessiveOverRelaxation)
            {
                fmt::println(out, "\tRelaxation Factor: {:12.6e}", params.relaxation_factor);
            }
        }
    }

    [[nodiscard]]
    auto run() const -> Solution
    {
        const auto b = problem.source.data();

        auto matelem = [&](const std::size_t i, const std::size_t j) -> T
        {
            return problem.operator_element(i, j);
        };

        switch (params.algorithm)
        {
            case AxbAlgorithm::LUP:
            {
                const auto start = std::chrono::high_resolution_clock::now();

                const auto dim = problem.grid.points.size();
                auto A = Matrix<T>::from_func(
                    static_cast<std::size_t>(dim),
                    [&](const auto i, const auto j) -> T
                    {
                        return problem.operator_element(i, j);
                    }
                );


                const auto [P, lu_result] = lup_factor_inplace<T>(A);
                auto x = lup_solve<T>(A, P, b);
                std::vector<T> tmp(x.size());
                std::vector<T> residual{ b.cbegin(), b.cend() };
                gemv<T, MatrixSymmetry::Upper>(A, x, tmp, T{ 1 });
                gemv<T, MatrixSymmetry::Lower, Diag::Unit>(A, tmp, residual, T{ -1 }, T{ 1 });
                const auto residual_error = max_abs(residual);

                const auto end = std::chrono::high_resolution_clock::now();

                if (lu_result == LUResult::SmallPivotEncountered)
                {
                    std::cerr << fmt::format(
                            fmt::emphasis::bold | fg(fmt::color::red),
                            "Error: Small Pivot Encountered"
                        )
                        << std::endl;
                }

                return {
                    *this,
                    Matrix<T>(
                        static_cast<std::size_t>(problem.grid.points.NX),
                        static_cast<std::size_t>(problem.grid.points.NY),
                        std::move(x)
                    ),
                    residual_error,
                    std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
                };
            }
            case AxbAlgorithm::PointJacobi:
            {
                const auto start = std::chrono::high_resolution_clock::now();
                auto result = point_jacobi_sparse<T>(problem, b, params.iter_settings);
                const auto end = std::chrono::high_resolution_clock::now();

                return {
                    *this,
                    Matrix<T>(
                        static_cast<std::size_t>(problem.grid.points.NX),
                        static_cast<std::size_t>(problem.grid.points.NY),
                        std::move(result.x)
                    ),
                    result.residual_error,
                    std::chrono::duration_cast<std::chrono::nanoseconds>(end - start),
                    result.converged,
                    result.relative_error,
                    result.iters
                };
            }
            case AxbAlgorithm::GaussSeidel:
            {
                const auto start = std::chrono::high_resolution_clock::now();
                auto result = gauss_seidel_sparse<T>(problem, b, params.iter_settings);
                const auto end = std::chrono::high_resolution_clock::now();

                return {
                    *this,
                    Matrix<T>(
                        static_cast<std::size_t>(problem.grid.points.NX),
                        static_cast<std::size_t>(problem.grid.points.NY),
                        std::move(result.x)
                    ),
                    result.residual_error,
                    std::chrono::duration_cast<std::chrono::nanoseconds>(end - start),
                    result.converged,
                    result.relative_error,
                    result.iters
                };
            }
            case AxbAlgorithm::SuccessiveOverRelaxation:
            {
                const auto start = std::chrono::high_resolution_clock::now();
                auto result = successive_over_relaxation_sparse<T>(problem, b, params.relaxation_factor, params.iter_settings);
                const auto end = std::chrono::high_resolution_clock::now();

                return {
                    *this,
                    Matrix<T>(
                        static_cast<std::size_t>(problem.grid.points.NX),
                        static_cast<std::size_t>(problem.grid.points.NY),
                        std::move(result.x)
                    ),
                    result.residual_error,
                    std::chrono::duration_cast<std::chrono::nanoseconds>(end - start),
                    result.converged,
                    result.relative_error,
                    result.iters
                };
            }
            default:
                throw std::invalid_argument("Invalid algorithm");
        }
    }

    [[nodiscard]]
    static auto from_file(std::istream& input) -> Project02
    {
        switch (const auto algorithm = read_axb_algorithm(input))
        {
            case AxbAlgorithm::LUP:
            {
                return {
                    .params = {algorithm},
                    .problem = IsotropicSteadyStateDiffusion2D<T>::from_file(input),
                };
            }
            case AxbAlgorithm::PointJacobi:
            case AxbAlgorithm::GaussSeidel:
            {
                return {
                    .params = {
                        algorithm,
                        FixedPointIterSettings<T>::template from_file<ParamOrder::MaxIterFirst>(input),
                    },
                    .problem = IsotropicSteadyStateDiffusion2D<T>::from_file(input),
                };
            }
            case AxbAlgorithm::SuccessiveOverRelaxation:
            {
                const auto settings = FixedPointIterSettings<T>::template from_file<ParamOrder::MaxIterFirst>(input);
                const auto relaxation_factor = read_positive_value<T>(input, "relaxation factor");
                if (relaxation_factor <= 1.0)
                {
                    throw std::runtime_error(
                        fmt::format("SOR relaxation factor must be larger than 1: {}", relaxation_factor)
                    );
                }

                return {
                    .params = {
                        algorithm,
                        settings,
                        relaxation_factor,
                    },
                    .problem = IsotropicSteadyStateDiffusion2D<T>::from_file(input),
                };
            }
            default:
            {
                throw std::runtime_error("Invalid algorithm");
            }
        }
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        Project02<T>,
        params,
        problem
    )
};


template<std::floating_point T>
[[nodiscard]]
auto read_input_file(const std::string &filename, const bool from_json = false) -> Project02<T> {
    const auto input_filepath = std::filesystem::path{filename};


    if (input_filepath.empty()) {
        throw std::runtime_error(
                fmt::format("Input file does not exist: {}", input_filepath.string())); // Indicate an error occurred
    }

    std::ifstream input{input_filepath};

    if (from_json) {
        json data = json::parse(input);
        return data.template get<Project02<T>>();
    }

    if (!input.is_open()) {
        throw std::runtime_error(
                fmt::format("Could not open input file: {}", input_filepath.string())); // Indicate an error occurred
    }

    try {
        const auto p = Project02<T>::from_file(input);
        input.close();
        return p;
    }
    catch (const std::exception &) {
        input.close();
        throw;
    }
}

#endif //PROJECT02_H
