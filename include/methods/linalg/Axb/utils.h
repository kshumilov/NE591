#ifndef LINALG_AXB_UTILS_H
#define LINALG_AXB_UTILS_H

#include <concepts>
#include <vector>

#include <fmt/format.h>

#include "methods/linalg/matrix.h"
#include "methods/linalg/utils/math.h"


template<std::floating_point DType>
[[nodiscard]]
constexpr auto is_diag_nonzero(const Matrix<DType>& A) -> bool {
    for (std::size_t i{}; i < A.rows(); ++i) {
        if (isclose(A[i, i], DType{})) {
            return false;
        }
    }
    return true;
}


template<std::floating_point DType>
struct IterativeAxbResult {
    std::vector<DType> x{};
    DType error{};
    bool converged{};
    int iters{};

    [[nodiscard]]
    constexpr auto to_string() const -> std::string {
        if (converged) {
            return fmt::format("Converged at iteration #{:d} with error: {:12.6e}", iters, error);
        }
        return fmt::format("Failed to converge in {:d} iterations with final error: {:12.6e}", iters, error);
    }
};

#endif //LINALG_AXB_UTILS_H
