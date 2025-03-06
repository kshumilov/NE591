#ifndef GRID_H
#define GRID_H

#include <concepts>

#include <fmt/core.h>

template<std::floating_point T>
class SpatialGrid1D {
  public:
    [[nodiscard]]
    explicit constexpr SpatialGrid1D(std::vector<T>&& points_)
      : m_points{ std::move(points_) }
    {
      if (m_points.size() < 2)
      {
        throw std::invalid_argument(
          "`points` must have at least 2 elements: {} < 2",
          m_points.size()
        );
      }

      std::sort(m_points.begin(), m_points.end());
    }

    [[nodiscard]]
    constexpr auto num_cells() const -> int
    {
      return static_cast<int>(m_cells.size());
    }

    [[nodiscard]]
    constexpr auto num_points() const -> int
    {
      return static_cast<int>(m_points.size());
    }

  private:
    std::vector<T> m_points{};
    std::vector<T> m_cells{};

};

#endif //GRID_H
