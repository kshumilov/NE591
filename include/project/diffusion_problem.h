#ifndef DIFFUSION_PROBLEM_H
#define DIFFUSION_PROBLEM_H

#include <concepts>
#include <ostream>
#include <stdexcept>
#include <string>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "methods/array.h"
#include "methods/linalg/matrix.h"
#include "methods/utils/io.h"

using json = nlohmann::json;
using namespace std::literals;


template<std::floating_point scalar_t>
struct Space2D
{
    scalar_t X{ 1 }; //* Space extent in X directions */
    scalar_t Y{ 1 }; //* Space extent in Y directions */

    constexpr Space2D() = default;

    constexpr Space2D(const Space2D&) = default;

    constexpr Space2D(Space2D&&) = default;


    constexpr Space2D(const scalar_t a_, const scalar_t b_) : X{ a_ }
                                                          , Y{ b_ } { validate(); }


    constexpr auto validate() const -> void
    {
        if (not is_valid())
        {
            throw std::invalid_argument("Space dimensions must be positive");
        }
    }


    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool { return X >= 0 && Y >= 0; }


    [[nodiscard]] constexpr auto to_string(const int label_width = 40) const noexcept -> std::string
    {
        return fmt::format("{:.<{}}: {:12.6e} x {:12.6e}", "Space Dimensions, a x b", label_width, X, Y);
    }


    auto echo(std::ostream& out) const -> void { out << to_string() << std::endl; }


    [[nodiscard]]
    static auto from_file(std::istream& input) -> Space2D
    {
        const auto a = read_positive_value<scalar_t>(input, "a"sv);
        const auto b = read_positive_value<scalar_t>(input, "b"sv);

        return { a, b };
    }


    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Space2D, X, Y)
};


struct GridPoints2D
{
    int NX{ 1 }; // #points in X directions, not including boundary (+2)
    int NY{ 1 }; // #points in Y directions, not including boundary (+2)

    constexpr GridPoints2D() = default;

    constexpr GridPoints2D(const GridPoints2D&) = default;

    constexpr GridPoints2D(GridPoints2D&&) = default;


    constexpr GridPoints2D(const int M_, const int N_) : NX{ M_ }
                                                     , NY{ N_ } { validate(); }


    constexpr void validate() const
    {
        if (not is_valid())
        {
            throw std::invalid_argument("Space dimensions must be positive");
        }
    }


    [[nodiscard]] constexpr bool is_valid() const noexcept { return NX >= 1 && NY >= 1; }


    [[nodiscard]] constexpr std::string to_string(const int label_width = 40) const noexcept
    {
        return fmt::format("{:.<{}}: {:d} x {:d}", "Non-Zero Grid Points, M x N", label_width, NX, NY);
    }


    [[nodiscard]] constexpr int size() const noexcept { return NX * NY; }

    auto echo(std::ostream& out) const -> void { out << to_string() << std::endl; }


    [[nodiscard]]
    static auto from_file(std::istream& input) -> GridPoints2D
    {
        const auto M = read_positive_value<int>(input, "M");
        const auto N = read_positive_value<int>(input, "N");
        return { M, N };
    }


    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GridPoints2D, NX, NY)
};


template<std::floating_point scalar_t>
struct Grid2D
{
    Space2D<scalar_t> space{};
    GridPoints2D points{};

    constexpr Grid2D() = default;

    constexpr Grid2D(const Grid2D&) = default;

    constexpr Grid2D(Grid2D&&) = default;


    constexpr Grid2D(const Space2D<scalar_t>& space_, const GridPoints2D& points_) : space{ space_ }
      , points{ points_ }
    {
        validate();
    }


    constexpr void validate() const
    {
        space.validate();
        points.validate();
    };

    [[nodiscard]] constexpr scalar_t dx() const noexcept { return step<scalar_t>(space.X, points.NX + 1); }

    [[nodiscard]] constexpr scalar_t dy() const noexcept { return step<scalar_t>(space.Y, points.NY + 1); }

    [[nodiscard]] constexpr bool is_valid() const noexcept { return space.is_valid() && points.is_valid(); }


    [[nodiscard]] constexpr std::string to_string(const int label_width = 40) const noexcept
    {
        return fmt::format("{}\n{}", space.to_string(label_width), points.to_string(label_width));
    }


    auto echo(std::ostream& out) const -> void { out << to_string() << std::endl; }


    [[nodiscard]]
    static auto from_file(std::istream& input) -> Grid2D
    {
        const auto space = Space2D<scalar_t>::from_file(input);
        const auto points = GridPoints2D::from_file(input);
        return { space, points };
    }


    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Grid2D, space, points)
};


template<std::floating_point DType>
struct IsotropicSteadyStateDiffusion2D
{
    Grid2D<DType> grid{};
    DType diffusion_coefficient{ 1 };
    DType absorption_scattering{ 0 };
    Matrix<DType> source{ 1, 1, DType{} };


    void validate() const
    {
        grid.validate();

        if (diffusion_coefficient <= 0)
        {
            throw std::invalid_argument("`diffusion_coefficient` must be positive");
        }

        if (absorption_scattering < 0)
        {
            throw std::invalid_argument("`absorption_scattering` must be none-negative");
        }

        if (source.size() != static_cast<std::size_t>(grid.points.size()))
        {
            throw std::invalid_argument("`source` must be the same size as the grid");
        }

        if (not std::ranges::all_of(
            source.data().cbegin(),
            source.data().cend(),
            [](const auto v) { return v >= DType{}; }
        ))
        {
            throw std::invalid_argument("`source` must be non-negative");
        }
    }


    [[nodiscard]] constexpr auto operator_element(const std::size_t I, const std::size_t J) const -> DType
    {
        if (I == J)
            return diagonal_element(I);

        const auto [i_q, j_q] = unravel2d(I, N());
        const auto [i_phi, j_phi] = unravel2d(J, N());

        // -D(\phi{i + 1, j} + \phi{i - 1, j}) / \delta ^ 2
        if (j_phi == j_q && ((0U < i_q && i_q == i_phi + 1U) || (i_q + 1U < M() && i_q + 1U == i_phi)))
            return horizontal_element();

        // -D(\phi{i, j + 1} + \phi{i, j - 1}) / \gamma ^ 2
        if (i_phi == i_q && ((0U < j_q && j_q == j_phi + 1U) || (j_q + 1U < N() && j_q + 1U == j_phi)))
            return vertical_element();

        return DType{};
    }

    auto matvec(std::span<const DType> x, std::span<DType> y, const DType alpha, const DType beta) const -> void
    {
        const auto dim = static_cast<std::size_t>(grid.points.size());
        assert(dim == x.size());
        assert(dim == y.size());

        for (std::size_t i = 0; i < dim; ++i)
        {
            y[i] = alpha * rowvec(i, x) + beta * y[i];
        }
    }

    auto rowvec(const std::size_t i, std::span<const DType> x) const noexcept -> DType
    {
        DType dot_prod{};

        const auto nonzero = nonzero_row_elems(i);
        for (const auto& [j, value] : nonzero)
        {
            dot_prod += value * x[j];
        }

        return dot_prod;
    }

    auto nonzero_row_elems(const std::size_t i) const -> std::vector<std::pair<std::size_t, DType>>
    {
        std::vector<std::pair<std::size_t, DType>> nonzero{};
        nonzero.emplace_back(std::make_pair(i, diagonal_element(i)));

        const auto [i_q, j_q] = unravel2d(i, N());

        if (0U < i_q)
        {
            const auto j = ravel2d(i_q - 1U, j_q, N());
            nonzero.emplace_back(std::make_pair(j, horizontal_element()));
        }

        if (i_q + 1U < M())
        {
            const auto j = ravel2d(i_q + 1U, j_q, N());
            nonzero.emplace_back(std::make_pair(j, horizontal_element()));
        }

        if (0U < j_q)
        {
            const auto j = ravel2d(i_q, j_q - 1U, N());
            nonzero.emplace_back(std::make_pair(j, vertical_element()));
        }

        if (j_q + 1U < N())
        {
            const auto j = ravel2d(i_q, j_q + 1U, N());
            nonzero.emplace_back(std::make_pair(j, vertical_element()));
        }

        return nonzero;
    }

    [[nodiscard]] constexpr
    auto M() const noexcept -> std::size_t
    {
        return static_cast<std::size_t>(grid.points.NX);
    }

    [[nodiscard]] constexpr
    auto N() const noexcept -> std::size_t
    {
        return static_cast<std::size_t>(grid.points.NY);
    }

    [[nodiscard]] constexpr
    auto horizontal_element() const noexcept -> DType
    {
        return -diffusion_coefficient / (grid.dx() * grid.dx());
    }

    [[nodiscard]] constexpr
    auto vertical_element() const noexcept -> DType
    {
        return -diffusion_coefficient / (grid.dy() * grid.dy());
    }

    [[nodiscard]] constexpr
    auto diagonal_element(const std::size_t) const noexcept -> DType
    {
        return -DType{ 2.0 } * (horizontal_element() + vertical_element()) + absorption_scattering;
    }

    [[nodiscard]] constexpr std::string to_string(const int label_width = 40) const noexcept
    {
        return fmt::format(
            "{}\n"
            "{:.<{}}: {}\n"
            "{:.<{}}: {}\n"
            "{:.<{}}:\n{}",
            grid.to_string(label_width),
            "Diffusion Coefficient, D",
            label_width,
            diffusion_coefficient,
            "Macroscopic Removal Cross Section, Sa",
            label_width,
            absorption_scattering,
            "Source, q",
            label_width,
            source.to_string()
        );
    }

    auto echo(std::ostream& out) const -> void { out << to_string() << std::endl; }


    [[nodiscard]]
    static auto from_file(std::istream& input) -> IsotropicSteadyStateDiffusion2D
    {
        const auto grid = Grid2D<DType>::from_file(input);

        return {
            .grid = grid,
            .diffusion_coefficient = read_positive_value<DType>(input, "D"),
            .absorption_scattering = read_nonnegative_value<DType>(input, "Sa"),
            .source = read_matrix<DType, MatrixSymmetry::General>(input, grid.points.NX, grid.points.NY)
        };
    }


    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        IsotropicSteadyStateDiffusion2D<DType>,
        grid,
        diffusion_coefficient,
        absorption_scattering,
        source
    )
};

#endif // DIFFUSION_PROBLEM_H
