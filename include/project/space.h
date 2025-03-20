//
// Created by Kirill Shumilov on 3/13/25.
//

#ifndef PROBLEM_H
#define PROBLEM_H

#include <concepts>
#include <stdexcept>
#include <utility>

#include <fmt/core.h>

#include "methods/stencil.h"
#include "methods/special.h"
#include "methods/utils/io.h"
#include "methods/linalg/utils/io.h"


template<std::floating_point T>
class UniformGrid1D
{
    public:
        static constexpr int MIN_POINTS{ 2 };

        constexpr UniformGrid1D() = default;

        explicit constexpr UniformGrid1D(const int num_points_)
            : m_num_points(num_points_) { validate(); }

        constexpr UniformGrid1D(const T length_, const int num_points_)
            : m_extent{ length_ }
          , m_num_points{ num_points_ }
        {
            validate();
        }


        constexpr auto extent() const
        {
            return m_extent;
        }


        constexpr auto num_points() const
        {
            return m_num_points;
        }


        constexpr auto num_inner_points() const
        {
            return num_points() - 2;
        }


        constexpr auto num_cells() const
        {
            return num_points() - 1;
        }


        constexpr auto num_inner_cells() const
        {
            return num_cells() > 2 ? num_cells() - 2 : 0;
        }


        constexpr auto step_size() const -> T
        {
            return m_extent / static_cast<T>(num_cells());
        }

        constexpr auto operator()(int i) const -> T
        {
            return i * step_size();
        }

    private:
        T m_extent{ 1 };
        int m_num_points{ 2 };


        constexpr auto validate() const
        {
            if (m_extent <= 0)
            {
                throw std::invalid_argument(fmt::format("`extent` must be non-positive: {} < 0", m_extent));
            }

            if (num_points() < MIN_POINTS)
            {
                throw std::invalid_argument(fmt::format("`num_points` must be greater than two: {} < 2", m_num_points));
            }
        }
};


template<std::floating_point T>
struct UniformGrid2D
{
    UniformGrid1D<T> grid_x{};
    UniformGrid1D<T> grid_y{};

    [[nodiscard]]
    constexpr auto num_points() const
    {
        return x().num_points() * y().num_points();
    }


    [[nodiscard]]
    constexpr auto num_inner_points() const
    {
        return x().num_inner_points() * y().num_inner_points();
    }


    [[nodiscard]]
    constexpr auto num_cells() const
    {
        return x().num_cells() * y().num_cells();
    }


    [[nodiscard]]
    constexpr auto num_inner_cells() const
    {
        return x().num_inner_cells() * y().num_inner_cells();
    }


    [[nodiscard]]
    constexpr auto operator()(const int i, const int j) const
    {
        return std::make_pair(x(i), y(j));
    }


    [[nodiscard]]
    constexpr auto dx() const
    {
        return x().step_size();
    }


    [[nodiscard]]
    constexpr auto dy() const
    {
        return y().step_size();
    }


    [[nodiscard]]
    constexpr const auto& x() const
    {
        return grid_x;
    }


    [[nodiscard]]
    constexpr auto x(const auto i) const
    {
        return x()(i);
    }


    [[nodiscard]]
    constexpr const auto& y() const
    {
        return grid_y;
    }


    [[nodiscard]]
    constexpr auto y(const auto i) const
    {
        return y()(i);
    }

    [[nodiscard]]
    constexpr auto is_square() const
    {
        return x().num_points() == y().num_points();
    }


    [[nodiscard]]
    constexpr std::string to_string(const int label_width = 40) const noexcept
    {
        return fmt::format(
            "{1:.<{0}}: {2:12.6e} x {3:12.6e}\n"
            "{4:.<{0}}: {5:d} x {6:d}",
            label_width,
            "Space Dimensions, a x b",
            x().extent(),
            y().extent(),
            "Non-Zero Grid Points, M x N",
            x().num_inner_points(),
            y().num_inner_points()
        );
    }


    [[nodiscard]]
    constexpr auto build_indexer() const
    {
        return Indexer2D{ x().num_points(), y().num_points() };
    }


    [[nodiscard]]
    constexpr auto build_inner_indexer() const
    {
        return Indexer2D{ x().num_inner_points(), y().num_inner_points() };
    }


    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
        const auto X = read_positive_value<T>(input, "a");
        const auto Y = read_positive_value<T>(input, "b");
        const auto NX = read_positive_value<int>(input, "M");
        const auto NY = read_positive_value<int>(input, "N");

        return UniformGrid2D{ { X, NX + 2 }, { Y, NY + 2 } };
    }
};


template<std::floating_point T>
struct RectangularRegion
{
    UniformGrid2D<T> grid{};
    T diffusion_coefficient{ 1 };
    T absorption_scattering{ 0 };
    Matrix<T> source{ UniformGrid1D<T>::MIN_POINTS, UniformGrid1D<T>::MIN_POINTS, T{} };

    constexpr RectangularRegion()
    {
        validate();
    }

    constexpr RectangularRegion(
        const UniformGrid2D<T>& grid_,
        T D_,
        T Sa_,
        Matrix<T>&& q_
    ) : grid{ grid_ },
        diffusion_coefficient{ D_ },
        absorption_scattering{ Sa_ },
        source{ std::move(q_) }
    {
       validate();
    }


    [[nodiscard]]
    constexpr RectangularRegion
    (
        const UniformGrid2D<T>& grid_,
        Gaussian2D<T> gaussian_2d,
        const T D,
        const T Sa
    ) : RectangularRegion{
          grid_,
          D,
          Sa,
          Matrix<T>::from_func(
              grid_.x().num_inner_points(),
              grid_.y().num_inner_points(),
              [&](const int i, const int j)
              {
                  return gaussian_2d(grid_(i + 1, j + 1));
              }
          )
      }
    {}


    constexpr auto validate() const
    {
        if (grid.num_inner_points() < 1)
        {
            throw std::invalid_argument(
                fmt::format("number of inner grid points must be at least one: {} < 1", grid.num_inner_points())
            );
        }

        if (diffusion_coefficient <= 0)
        {
            throw std::invalid_argument(
                fmt::format("`diffusion_coefficient` must be positive: {} <= 0", diffusion_coefficient)
            );
        }

        if (absorption_scattering < 0)
        {
            throw std::invalid_argument(
                fmt::format("`absorption_scattering` must be non-negative: {} < 0", absorption_scattering)
            );
        }

        if (source.rows() != static_cast<std::size_t>(grid.x().num_inner_points()) or
            source.cols() != static_cast<std::size_t>(grid.y().num_inner_points()))
        {
            throw std::invalid_argument(
                fmt::format(
                    "`source` must be the same size as the number of inner grid points: {} != {}",
                    source.size(),
                    grid.num_inner_points()
                )
            );
        }

        if (not std::ranges::all_of(
            source.data().cbegin(),
            source.data().cend(),
            [](const auto v) { return v >= T{}; }
        ))
        {
            throw std::invalid_argument("`source` must be non-negative");
        }
    }


    [[nodiscard]]
    constexpr auto build_stencil() const
    {
        const auto horizontal = -diffusion_coefficient / (grid.dx() * grid.dx());
        const auto vertical = -diffusion_coefficient / (grid.dy() * grid.dy());
        const auto center = -T{ 2.0 } * (horizontal + vertical) + absorption_scattering;

        return ConstantStencil2D<T>{
            .shape = grid.build_indexer(),
            .m_top = horizontal,
            .m_bottom = horizontal,
            .m_left = vertical,
            .m_right = vertical,
            .m_center = center
        };
    }

    [[nodiscard]]
    constexpr auto build_matrix() const
    {
        const auto stencil = build_stencil();
        return stencil.build_matrix();
    }


    [[nodiscard]] constexpr std::string to_string(const int label_width = 40) const noexcept
    {
        return fmt::format(
            "{1}\n"
            "{2:.<{0}}: {3}\n"
            "{4:.<{0}}: {5}\n"
            "{6:.<{0}}: {7}",
            label_width,
            grid.to_string(label_width),
            "Diffusion Coefficient, D",
            diffusion_coefficient,
            "Macroscopic Removal Cross Section, Sa",
            absorption_scattering,
            "Source, q",
            source.rows() <= 8
                ? fmt::format("{}\n{}", source.shape_info(), source.to_string())
                : source.shape_info()
        );
    }


    [[nodiscard]]
    static auto from_file(std::istream& input)
    {
        const auto grid = UniformGrid2D<T>::from_file(input);
        return RectangularRegion{
            grid,
            read_positive_value<T>(input, "D"),
            read_nonnegative_value<T>(input, "Sa"),
            read_matrix<T, MatrixSymmetry::General>(
                input, grid.x().num_inner_points(), grid.y().num_inner_points()
            )
        };
    }
};


#endif // PROBLEM_H
