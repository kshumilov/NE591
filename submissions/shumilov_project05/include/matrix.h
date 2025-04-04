#ifndef MATRIX_H
#define MATRIX_H

#include <ranges>
#include <span>
#include <type_traits>

#include <fmt/ranges.h>

#include "shape.h"


template<class T>
struct MatrixView
{
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using reference = T&;

    std::span<T> data{};
    Shape2D shape{};

    MatrixView(std::span<T> data_, const Shape2D& shape_)
        : data{ data_ }
        , shape{ shape_ }
    {
        // assert(data.size() == static_cast<std::size_t>(rows * cols));
    }

    [[nodiscard]]
    constexpr auto ravel(const int row, const int col) const
    {
        return row * shape.cols() + col;
    }

    [[nodiscard]]
    constexpr reference operator[](const int row, const int col) const
    {
        return data[static_cast<std::size_t>(ravel(row, col))];
    }

    [[nodiscard]]
    auto to_string(const std::string_view sep = " ") const -> std::string
    {
        // Format each row
        const auto lines =
            data | std::views::as_const | std::views::chunk(shape.cols()) |
            std::views::transform([sep](const auto& c) { return fmt::format("[{: 12.6e}]", fmt::join(c, sep)); });

        return fmt::format("[{}]", fmt::join(lines, " \n "));
    }
};
#endif //MATRIX_H
