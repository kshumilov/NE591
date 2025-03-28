#ifndef LINALG_AXB_LINEAR_SYSTEM_H
#define LINALG_AXB_LINEAR_SYSTEM_H

#include <concepts>
#include <vector>

#include "utils/io.h"

#include "methods/linalg/matrix.h"

template<std::floating_point T>
struct LinearSystem
{
    Matrix<T> A{};
    std::vector<T> b{};

    [[nodiscard]]
    constexpr LinearSystem(Matrix<T>&& A_, std::vector<T>&& b_)
        : A{ std::move(A_) }
        , b{ std::move(b_) }
    {
        if (not A.is_square())
        {
            throw std::invalid_argument("`A` must be a square matrix: ()");
        }

        if (not LinearSystem::matches_shape(A, b))
        {
            throw std::invalid_argument(
                fmt::format("Shape mismatch: ({}, {}) & ({})", A.rows(), A.cols(), b.size())
            );
        }
    }

    [[nodiscard]]
    static auto matches_shape(const Matrix<T>& A, std::vector<T>& b)
    {
        return A.rows() == b.size();
    }

    [[nodiscard]]
    constexpr auto rank() const
    {
        return static_cast<int>(A.rows());
    }

    [[nodiscard]]
    constexpr auto residual(const std::vector<T>& x, std::vector<T>& r) const
    {
        std::copy(this->b.cbegin(), this->b.cend(), r.begin());
        gemv<T>(A, x, r, -T{ 1 }, T{ 1 });
    }

    [[nodiscard]]
    static auto from_file(std::istream& input) -> LinearSystem
    {
        const auto rank = static_cast<std::size_t>(read_positive_value<int>(input, "Matrix rank n"));
        return LinearSystem{
            read_square_matrix<T, MatrixSymmetry::General>(input, rank),
            read_vector<T>(input, rank)
        };
    }
};


template<std::floating_point T>
struct fmt::formatter<LinearSystem<T>>
{

    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const LinearSystem<T>& system, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(),
            "Matrix, A: {:F: 14.8e}\n\n"
            "RHS Vector, b:\n[{: 14.8e}]",
            system.A, fmt::join(system.b, " ")
        );
    }
};
#endif //LINALG_AXB_LINEAR_SYSTEM_H
