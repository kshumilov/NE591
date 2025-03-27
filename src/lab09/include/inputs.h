#ifndef INPUTS_H
#define INPUTS_H

#include <concepts>

#include <fmt/core.h>

#include "methods/linalg/matrix.h"
#include "methods/optimize.h"
#include "methods/utils/io.h"
#include "methods/linalg/utils/io.h"

#include "methods/linalg/Axb/conjugate_gradient.h"


template<std::floating_point T>
struct Inputs
{
    FixedPointIterSettings<T> iter_settings{};
    Matrix<T> A{};
    std::vector<T> b{};

    [[nodiscard]]
    constexpr Inputs(FixedPointIterSettings<T>&& iter_settings_, Matrix<T>&& A_, std::vector<T>&& b_)
        : iter_settings{ std::move(iter_settings_) }
        , A{ std::move(A_) }
        , b{ std::move(b_) }
    {
        CGState<T>::validate_A(A);

        if (A.rows() != b.size())
        {
            throw std::invalid_argument(
                fmt::format(
                    "`A` must have the same number of rows as the size of `b`: {} != {}",
                    A.rows(),
                    b.size()
                )
            );
        }
    }

    [[nodiscard]]
    constexpr auto to_string() const
    {
        return fmt::format(
            "{2:^{0}}\n"
            "{8}\n"
            "{3:.<{1}}: {4}\n"
            "{5:.<{1}}: {6}\n"
            "{7:-<{0}}\n",
            COLUMN_WIDTH, 40,
            "Inputs",

            "Matrix, A",
            A.rows() <= 8
                ? fmt::format("{}\n{}", A.shape_info(), A.to_string())
                : A.shape_info(),
            "RHS Vector, b",
            b.size() <= 8
                ? fmt::format("<{:d}, {:s}>\n{}", b.size(), typeid(T).name(), b)
                : fmt::format("<{:d}, {:s}>", b.size(), typeid(T).name()),
            "",
            iter_settings.to_string()
        );

    }

    [[nodiscard]]
    static auto from_file(std::istream& input) -> Inputs<T>
    {
        auto settings = FixedPointIterSettings<T>::from_file(input);
        const auto rank = static_cast<std::size_t>(read_positive_value<int>(input, "Matrix rank n"));
        return Inputs<T>{
            std::move(settings),
            read_square_matrix<T, MatrixSymmetry::General>(input, rank),
            read_vector<T>(input, rank)
        };
    }

    [[nodiscard]]
    static auto from_file(const std::string& filename) -> Inputs<T>
    {
        const auto input_filepath = std::filesystem::path{ filename };

        if (input_filepath.empty())
        {
            throw std::runtime_error(
                fmt::format("Input file does not exist: {}", input_filepath.string())
            ); // Indicate an error occurred
        }

        std::ifstream input{ input_filepath };

        if (!input.is_open())
        {
            throw std::runtime_error(
                fmt::format("Could not open input file: {}", input_filepath.string())
            ); // Indicate an error occurred
        }

        return Inputs<T>::from_file(input);
    }
};

#endif //INPUTS_H
