#ifndef DIFFUSION_PROBLEM_H
#define DIFFUSION_PROBLEM_H

#include <concepts>
#include <stdexcept>
#include <ostream>
#include <string>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "methods/linalg/matrix.h"
#include "methods/array.h"
#include "methods/utils/io.h"


using json = nlohmann::json;
using namespace std::literals;


template<std::floating_point scalar_t>
struct Space2D {
  scalar_t X{1}; //* Space extent in X directions */
  scalar_t Y{1}; //* Space extent in Y directions */

  constexpr Space2D() = default;
  constexpr Space2D(const Space2D&) = default;
  constexpr Space2D(Space2D&&) = default;

  constexpr Space2D(const scalar_t a_, const scalar_t b_) : X{a_}, Y{b_}
  {
    validate();
  }

  constexpr void validate() const
  {
    if (not is_valid()) {
      throw std::invalid_argument("Space dimensions must be positive");
    }
  }

  [[nodiscard]] constexpr bool is_valid() const noexcept {
    return X >= 0 && Y >= 0;
  }

  [[nodiscard]] constexpr std::string to_string(const int label_width = 40) const noexcept {
    return fmt::format("{:.<{}}: {:12.6e} x {:12.6e}", "Space Dimensions, a x b", label_width, X, Y);
  }

  auto echo(std::ostream& out) const -> void
  {
     out << to_string() << std::endl;
  }

  [[nodiscard]]
  static auto from_file(std::istream& input) -> Space2D
  {
    const auto a = read_positive_value<scalar_t>(input, "a"sv);
    const auto b = read_positive_value<scalar_t>(input, "b"sv);

    return {a, b};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Space2D, X, Y)
};


struct GridPoints2D {
  int NX{1}; // #points in X directions, not including boundary (+2)
  int NY{1}; // #points in Y directions, not including boundary (+2)

  constexpr GridPoints2D() = default;
  constexpr GridPoints2D(const GridPoints2D&) = default;
  constexpr GridPoints2D(GridPoints2D&&) = default;

  constexpr GridPoints2D(const int M_, const int N_) : NX{M_}, NY{N_}
  {
    validate();
  }

  constexpr void validate() const
  {
    if (not is_valid()) {
      throw std::invalid_argument("Space dimensions must be positive");
    }
  }

  [[nodiscard]] constexpr bool is_valid() const noexcept
  {
    return NX >= 1 && NY >= 1;
  }

  [[nodiscard]] constexpr std::string to_string(const int label_width = 40) const noexcept
  {
    return fmt::format("{:.<{}}: {:d} x {:d}", "Non-Zero Grid Points, M x N", label_width, NX, NY);
  }

  [[nodiscard]] constexpr int size() const noexcept
  {
    return NX * NY;
  }

  auto echo(std::ostream& out) const -> void
  {
    out << to_string() << std::endl;
  }

  [[nodiscard]]
  static auto from_file(std::istream& input) -> GridPoints2D
  {
    const auto M = read_positive_value<int>(input, "M");
    const auto N = read_positive_value<int>(input, "N");
    return {M,  N};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(GridPoints2D, NX, NY)
};


template<std::floating_point scalar_t>
struct Grid2D {
  Space2D<scalar_t> space{};
  GridPoints2D points{};

  constexpr Grid2D() = default;
  constexpr Grid2D(const Grid2D&) = default;
  constexpr Grid2D(Grid2D&&) = default;

  constexpr Grid2D(const Space2D<scalar_t>& space_, const GridPoints2D& points_) : space{space_}, points{points_}
  {
    validate();
  }

  constexpr void validate() const
  {
    space.validate();
    points.validate();
  };

  [[nodiscard]] constexpr scalar_t dx() const noexcept
  {
    return step<scalar_t>(space.X, points.NX + 1);
  }

  [[nodiscard]] constexpr scalar_t dy() const noexcept
  {
    return step<scalar_t>(space.Y, points.NY + 1);
  }

  [[nodiscard]] constexpr bool is_valid() const noexcept
  {
    return space.is_valid() && points.is_valid();
  }

  [[nodiscard]] constexpr std::string to_string(const int label_width = 40) const noexcept
  {
    return fmt::format("{}\n{}", space.to_string(label_width), points.to_string(label_width));
  }

  auto echo(std::ostream& out) const -> void
  {
    out << to_string() << std::endl;
  }

  [[nodiscard]]
  static auto from_file(std::istream& input) -> Grid2D
  {
    const auto space = Space2D<scalar_t>::from_file(input);
    const auto points = GridPoints2D::from_file(input);
    return {space, points};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Grid2D, space, points)
};


template<std::floating_point scalar_t>
struct IsotropicSteadyStateDiffusion2D {
  Grid2D<scalar_t> grid{};
  scalar_t diffusion_coefficient{1};
  scalar_t absorption_scattering{0};
  Matrix<scalar_t> source{1, 1, scalar_t{}};

  void validate() const
  {
    grid.validate();

    if (diffusion_coefficient <= 0) {
      throw std::invalid_argument("`diffusion_coefficient` must be positive");
    }

    if (absorption_scattering < 0) {
      throw std::invalid_argument("`absorption_scattering` must be none-negative");
    }

    if (source.size() != static_cast<std::size_t>(grid.points.size())) {
      throw std::invalid_argument("`source` must be the same size as the grid");
    }
  }

  [[nodiscard]] constexpr
  auto operator_element(const std::size_t I, const std::size_t J) const -> scalar_t
  {
    const auto M_ = static_cast<std::size_t>(grid.points.NX);
    const auto D_over_delta_sq = diffusion_coefficient / (grid.dx() * grid.dx());

    const auto N_ = static_cast<std::size_t>(grid.points.NX);
    const auto D_over_gamma_sq = diffusion_coefficient / (grid.dx() * grid.dy());

    const auto [i, j] = unravel2d(I, N_);
    const auto [k, l] = unravel2d(J, N_);

    if (((0U < i && i - 1U == k) || (i < M_ - 1U && i + 1U == k)) && j == l) return -D_over_delta_sq;
    if (i == k && ((0U < j && j - 1U == l) || (j < N_ - 1U && j + 1U == l))) return -D_over_gamma_sq;
    if (i == k && j == l) return 2.0 * (D_over_delta_sq + D_over_gamma_sq) + absorption_scattering;

    return scalar_t{};
  }

  [[nodiscard]] constexpr std::string to_string(const int label_width = 40) const noexcept
  {
    return fmt::format("{}\n"
                       "{:.<{}}: {}\n"
                       "{:.<{}}: {}\n"
                       "{:.<{}}:\n{}",
                       grid.to_string(label_width),
                       "Diffusion Coefficient, D", label_width, diffusion_coefficient,
                       "Macroscopic Removal Cross Section, Sa", label_width, absorption_scattering,
                       "Source, q", label_width, source.to_string());
  }

  auto echo(std::ostream& out) const -> void
  {
    out << to_string() << std::endl;
  }

  [[nodiscard]]
  static auto from_file(std::istream& input) -> IsotropicSteadyStateDiffusion2D
  {
    const auto grid = Grid2D<scalar_t>::from_file(input);

    return {
      .grid = grid,
      .diffusion_coefficient = read_nonnegative_value<scalar_t>(input, "D"),
      .absorption_scattering = read_nonnegative_value<scalar_t>(input, "Sa"),
      .source = read_matrix<scalar_t, MatrixSymmetry::General>(input, grid.points.NX, grid.points.NY),
    };
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(IsotropicSteadyStateDiffusion2D<scalar_t>, grid, diffusion_coefficient, absorption_scattering, source)
};

#endif //DIFFUSION_PROBLEM_H
