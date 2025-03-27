#ifndef PROJECT03_INPUTS_H
#define PROJECT03_INPUTS_H

#include <filesystem>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "project/space.h"

#include "methods/optimize.h"
#include "methods/linalg/Axb/algorithm.h"
#include "methods/linalg/Axb/io.h"

#include "mpi_utils.h"

template<std::floating_point T>
struct Lab10
{
    AxbAlgorithm algorithm{};
    FixedPointIterSettings<T> iter_settings{};
    T relaxation_factor{ 1 };
    RectangularRegion<T> region{};

    auto echo(std::ostream& output) {
        fmt::print(
            output,
            "{2:^{1}s}\n"
            "{4:.<{0}}: {5}\n"
            "{3:s}\n",
            40, 80,
            "Inputs", region.to_string(),
            "#Processes", get_world_size(MPI_COMM_WORLD)
        );

        fmt::println(
            output,
            "Selected Method: {}",
            algorithm
        );

        if (algorithm != AxbAlgorithm::LUP)
        {
            fmt::print(
                output,
                "{:}\n",
                iter_settings.to_string()
            );

            if (algorithm == AxbAlgorithm::SuccessiveOverRelaxation)
            {
                fmt::println(output, "\tRelaxation Factor: {:12.6e}", relaxation_factor);
            }
        }

        fmt::print(
            output,
            "================================================================================\n"
        );
    }

    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
        switch (const auto algorithm = read_axb_algorithm(input))
        {
            case AxbAlgorithm::LUP:
            {
                return Lab10{
                    algorithm,
                    FixedPointIterSettings<T>{},
                    T{ 1 },
                    RectangularRegion<T>::from_file(input),
                };
            }
            case AxbAlgorithm::PointJacobi:
            case AxbAlgorithm::GaussSeidel:
            {
                return Lab10{
                    algorithm,
                    FixedPointIterSettings<T>::template from_file<ParamOrder::MaxIterFirst>(input),
                    T{ 1 },
                    RectangularRegion<T>::from_file(input),
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

                return Lab10{
                    algorithm,
                    settings,
                    relaxation_factor,
                    RectangularRegion<T>::from_file(input),
                };
            }
            default:
            {
                throw std::runtime_error("Invalid algorithm");
            }
        }
    }


    [[nodiscard]]
    static auto read_input_file(const std::string& filename)
    {
        const auto input_filepath = std::filesystem::path{ filename };

        if (input_filepath.empty())
        {
            throw std::runtime_error(
                fmt::format("Input file does not exist: {}", input_filepath.string())
            );
        }

        std::ifstream input{ input_filepath };

        if (!input.is_open())
        {
            throw std::runtime_error(
                fmt::format("Could not open input file: {}", input_filepath.string())
            ); // Indicate an error occurred
        }

        return Lab10::from_file(input);
    }
};


#endif //PROJECT03_H
