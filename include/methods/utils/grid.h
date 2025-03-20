#ifndef GRID_INDEXER_2D_H
#define GRID_INDEXER_2D_H

#include <stdexcept>  // invalid_argument
#include <utility>    // pair
#include <map>
#include <ranges>

#include <fmt/format.h>  // fmt::format

enum class Layout2D
{
    RowMajor, ColMajor
};

enum class Direction2D
{
    Top,
    Bottom,
    Left,
    Right,
};

template<>
struct fmt::formatter<Direction2D, char>
{
    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }


    template<class FmtContext>
    constexpr auto format(const Direction2D val, FmtContext& ctx) const
    {
        switch (val)
        {
            case Direction2D::Top:
                return fmt::format_to(ctx.out(), "Top");
            case Direction2D::Bottom:
                return fmt::format_to(ctx.out(), "Bottom");
            case Direction2D::Left:
                return fmt::format_to(ctx.out(), "Left");
            case Direction2D::Right:
                return fmt::format_to(ctx.out(), "Right");
            default:
                std::unreachable();
        }
        return ctx.out();
    }
};



enum class ApplyOrdering
{
    Sequential, CheckerBoard,
};

template<typename T>
concept Index2D = std::same_as<T, int> || std::same_as<T, std::pair<int, int>>;

template<Layout2D layout = Layout2D::RowMajor>
struct Indexer2D
{
    int m_rows{ 1 };
    int m_cols{ 1 };

    [[nodiscard]]
    constexpr auto is_valid_row_idx(const int i) const
    {
        return 0 <= i and i < rows();
    }

    constexpr void validate_row_idx(const int i) const
    {
        if (not is_valid_row_idx(i))
        {
            throw std::invalid_argument(
                fmt::format("`i` must be in the range [0, {}): {}", rows(), i)
            );
        }
    }

    [[nodiscard]]
    constexpr auto is_valid_col_idx(const int j) const
    {
        return 0 <= j and j < rows();
    }

    constexpr void validate_col_idx(const int j) const
    {
        if (not is_valid_row_idx(j))
        {
            throw std::invalid_argument(
                fmt::format("`j` must be in the range [0, {}): {}", cols(), j)
            );
        }
    }

    [[nodiscard]]
    constexpr auto is_valid_idx(const int i, const int j) const
    {
        return is_valid_row_idx(i) and is_valid_col_idx(j);
    }

    [[nodiscard]]
    constexpr auto is_valid_inner_idx(const int i, const int j) const
    {
        return 0 < i && i < rows() - 1 and 0 < j && j < cols() - 1;
    }


    [[nodiscard]]
    constexpr auto is_valid_offset(const int offset) const
    {
        return 0 <= offset and offset < nelems();
    }


    constexpr void validate_offset(const int offset) const
    {
        if (not is_valid_offset(offset))
        {
            throw std::invalid_argument(
                fmt::format("`offset` must be in the range [0, {}): {}", nelems(), offset)
            );
        }
    }


    using Offset = int;
    using IndexPair = std::pair<int, int>;


    [[nodiscard]]
    constexpr Indexer2D(const int rows_, const int cols_)
        : m_rows{ rows_ }
      , m_cols{ cols_ }
    {
        if (rows() < 1)
        {
            throw std::invalid_argument(
                fmt::format("`rows` must be positive: {}", rows())
            );
        }

        if (cols() < 1)
        {
            throw std::invalid_argument(
                fmt::format("`cols` must be positive: {}", cols())
            );
        }
    }


    [[nodiscard]]
    constexpr explicit Indexer2D(const int rows_) : Indexer2D{ rows_, rows_ } {}


    [[nodiscard]]
    constexpr auto ravel(const int i, const int j) const
    {
        validate_row_idx(i);
        validate_row_idx(j);
        return ravel(i, j);
    }


    [[nodiscard]]
    constexpr auto ravel(const IndexPair& idx) const
    {
        validate_row_idx(idx.first);
        validate_row_idx(idx.second);
        return this->operator[](idx.first, idx.second);
    }


    [[nodiscard]]
    constexpr auto operator[](const int i, const int j) const
    {
        if constexpr (layout == Layout2D::RowMajor)
        {
            return i * m_cols + j;
        }
        else
        {
            return i + j * m_rows;
        }
    }


    [[nodiscard]]
    constexpr auto unravel(const Offset offset) const -> IndexPair
    {
        validate_offset(offset);
        return this->operator[](offset);
    }


    [[nodiscard]]
    constexpr auto operator[](const Offset offset) const
    {
        if constexpr (layout == Layout2D::RowMajor)
        {
            return std::make_pair(offset / cols(), offset % cols());
        }
        else
        {
            return std::make_pair(offset % rows(), offset / rows());
        }
    }


    [[nodiscard]]
    constexpr auto operator==(const Indexer2D& other) const noexcept
    {
        return rows() == other.rows() && cols() == other.cols();
    }


    [[nodiscard]]
    constexpr auto rows() const -> int
    {
        return m_rows;
    }


    [[nodiscard]]
    constexpr auto inner_rows() const -> int
    {
        if (rows() <= 2)
        {
            return 0;
        }

        return rows() - 2;
    }


    [[nodiscard]]
    constexpr auto cols() const -> int
    {
        return m_cols;
    }


    [[nodiscard]]
    constexpr auto inner_cols() const -> int
    {
        if (cols() <= 2)
        {
            return 0;
        }

        return cols() - 2;
    }


    [[nodiscard]]
    constexpr auto nelems() const -> int
    {
        return rows() * cols();
    }


    template<ApplyOrdering ordering>
    constexpr auto apply
    (
        std::invocable<int, int> auto f,
        const int left_offset = 0,
        const int right_offset = 0,
        const int top_offset = 0,
        const int bottom_offset = 0
    ) const -> void
    {
        const int i_start = std::min(std::max(0, bottom_offset), rows());
        const int i_end = std::max(std::min(rows() - top_offset, rows()), 0);

        const int j_start = std::min(std::max(0, left_offset), cols());
        const int j_end = std::max(std::min(cols() - right_offset, cols()), 0);

        if constexpr (ordering == ApplyOrdering::Sequential)
        {
            if constexpr (layout == Layout2D::RowMajor)
                for (const auto i : std::views::iota(i_start, i_end))
                {
                    for (const auto j : std::views::iota(j_start, j_end))
                    {
                        f(i, j);
                    }
                }
            else
                for (const auto j : std::views::iota(j_start, j_end))
                {
                    for (const auto i : std::views::iota(i_start, i_end))
                    {
                        f(i, j);
                    }
                }
        }
        else if constexpr (ordering == ApplyOrdering::CheckerBoard)
        {
            if constexpr (layout == Layout2D::RowMajor)
            {
                // Black Squares
                for (const auto i : std::views::iota(i_start, i_end))
                {
                    for (int j{ 2 - i % 2 }; j < j_end; j += 2)
                    {
                        f(i, j);
                    }
                }

                // Red Squares
                for (const auto i : std::views::iota(i_start, i_end))
                {
                    for (int j{ 1 + i % 2 }; j < j_end; j += 2)
                    {
                        f(i, j);
                    }
                }
            }
            else
            {
                // Black Squares
                for (const auto j : std::views::iota(j_start, j_end))
                {
                    for (int i{ 2 - j % 2 }; i < i_end; i += 2)
                    {
                        f(i, j);
                    }
                }

                // Red Squares
                for (const auto j : std::views::iota(j_start, i_end))
                {
                    for (std::size_t i{ 1 + j % 2 }; i < i_end; i += 2)
                    {
                        f(i, j);
                    }
                }
            }
        }
    }


    template<ApplyOrdering ordering = ApplyOrdering::Sequential>
    constexpr void apply_inner(std::invocable<int, int> auto f) const
    {
        apply<ordering>(f, 1, 1, 1, 1);
    }


    template<Direction2D direction>
    [[nodiscard]]
    constexpr auto has_neighbor(const int i, const int j) const
    {
        validate_row_idx(i);
        validate_col_idx(j);
        if constexpr (direction == Direction2D::Left)
        {
            return j > 0;
        }
        else if constexpr (direction == Direction2D::Right)
        {
            return j < cols() - 1;
        }
        else if constexpr (direction == Direction2D::Top)
        {
            return i < rows() - 1;
        }
        else if constexpr (direction == Direction2D::Bottom)
        {
            return i > 0;
        }
    }


    template<Direction2D direction>
    [[nodiscard]]
    constexpr auto get_neighbor(const int i, const int j) const -> std::optional<int>
    {
        validate_row_idx(i);
        validate_col_idx(j);

        if (not has_neighbor<direction>(i, j))
        {
            return std::nullopt;
        }

        int i_n{ i }, j_n{ j };

        if constexpr (direction == Direction2D::Left)
        {
            j_n -= 1;
        }
        else if constexpr (direction == Direction2D::Right)
        {
            j_n += 1;
        }
        else if constexpr (direction == Direction2D::Top)
        {
            i_n += 1;
        }
        else if constexpr (direction == Direction2D::Bottom)
        {
            i_n -= 1;
        }

        return std::make_optional(this->operator[](i_n, j_n));
    }


    template<Direction2D direction>
    [[nodiscard]]
    constexpr auto get_neighbor(const IndexPair& idx) const
    {
        return get_neighbor<direction>(idx.first, idx.second);
    }


    [[nodiscard]]
    constexpr auto get_neighbors(const IndexPair& idx) const
    {
        std::map<Direction2D, int> neighbors{};

        if (const auto n = get_neighbor<Direction2D::Top>(idx);
            n.has_value())
            neighbors.emplace(Direction2D::Top, n.value());

        if (const auto n = get_neighbor<Direction2D::Bottom>(idx);
            n.has_value())
            neighbors.emplace(Direction2D::Bottom, n.value());

        if (const auto n = get_neighbor<Direction2D::Left>(idx);
            n.has_value())
            neighbors.emplace(Direction2D::Left, n.value());

        if (const auto n = get_neighbor<Direction2D::Right>(idx);
            n.has_value())
            neighbors.emplace(Direction2D::Right, n.value());

        return neighbors;
    }

    [[nodiscard]]
    constexpr auto get_neighbors(const Offset offset) const
    {
        return get_neighbors(unravel(offset));
    }


    [[nodiscard]]
    constexpr auto get_inner_indexer() const
    {
        return Indexer2D<layout>{ m_rows - 2, m_cols - 2 };
    }
};

#endif // GRID_INDEXER_2D_H
