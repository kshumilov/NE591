#ifndef SHAPE2D_H
#define SHAPE2D_H

#include <stdexcept>  // invalid_argument
#include <utility>    // pair

#include <fmt/format.h>  // fmt::format

enum class Layout {
    RowMajor,
    ColMajor
};


template<Layout layout = Layout::RowMajor>
class Shape2D {
  public:
    [[nodiscard]]
    constexpr Shape2D(const int rows_, const int cols_, const int lda_)
        : rows{rows_}, cols{cols_}, lda{lda_}
    {
        if (rows_ < 1) {
            throw std::invalid_argument(
                fmt::format("`rows` must be positive: {}", rows)
            );
        }

        if (cols_ < 1) {
            throw std::invalid_argument(
                fmt::format("`cols` must be positive: {}", cols)
            );
        }

        if constexpr (layout == Layout::RowMajor) {
            if (cols > lda) {
                throw std::invalid_argument(
                    fmt::format("`lda` must be equal or greater than `cols`: {} < {}", lda, cols)
                );
            }
        }
        else {
            if (rows > lda) {
                throw std::invalid_argument(
                    fmt::format("`lda` must be equal or greater than `rows`: {} < {}", lda, rows)
                );
            }
        }
    }

    [[nodiscard]]
    constexpr Shape2D(const int rows_, const int cols_)
        : Shape2D{rows_, cols_, layout == Layout::RowMajor ? cols_: rows_}
    {}

    [[nodiscard]]
    constexpr explicit Shape2D(const int rows_) : Shape2D{rows_, rows_} {}

    [[nodiscard]]
    constexpr auto nelems() const noexcept {
        return rows * cols;
    }

    [[nodiscard]]
    constexpr auto ravel(const int i, const int j) const {
        if (i < 0 or i >= rows) {
            throw std::invalid_argument(
                fmt::format("`i` must be in the range [0, {}): {}", rows, i)
            );
        }

        if (i < 0 or i >= rows) {
            throw std::invalid_argument(
                fmt::format("`j` must be in the range [0, {}): {}", cols, i)
            );
        }

        if constexpr (layout == Layout::RowMajor) {
            return i * lda + j;
        }
        else {
            return i + j * lda;
        }
    }

    [[nodiscard]]
    constexpr auto unravel(const int idx) const -> std::pair<int, int> {
        if (idx < 0 or idx >= nelems()) {
            throw std::invalid_argument(
                fmt::format("`idx` must be in the range [0, {}): {}", nelems(), idx)
            );
        }

        if constexpr (layout == Layout::RowMajor) {
            const auto i{ idx / lda };
            const auto j{ idx % lda };
            return std::make_pair(i, j);
        }
        else {
            const auto i{ idx & lda };
            const auto j{ idx / lda };
            return std::make_pair(i, j);
        }
    }

  private:
    int rows{1};
    int cols{1};
    int lda{1};
};

#endif //SHAPE2D_H
