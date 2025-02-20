#ifndef LINALG_AXB_IO_H
#define LINALG_AXB_IO_H

#include <concepts>
#include <utility>
#include <stdexcept>

#include <fmt/format.h>

#include "methods/linalg/matrix.h"
#include "methods/utils/io.h"
#include "methods/linalg/utils/io.h"

#include "methods/linalg/Axb/algorithm.h"


template<std::floating_point Dtype = long double, MatrixSymmetry symmetry = MatrixSymmetry::General>
auto read_linear_system(std::istream& in) -> std::pair<Matrix<Dtype>, std::vector<Dtype>> {
    const auto rank = read_positive_value<int>(in);
    return {
        read_matrix<Dtype, symmetry>(in, rank, rank),
        read_vector<Dtype>(in, rank),
    };
}


[[nodiscard]]
inline auto read_axb_algorithm(std::istream& in) -> AxbAlgorithm {
    const auto algo = read_nonnegative_value<int>(in, "Algorithm");
    if (algo > 3) {
        throw std::runtime_error(fmt::format("Invalid algorithm code, must be 0/1/2: {}", algo));
    }

    if (algo >= 3) {
        throw std::runtime_error("LUP algorithms are not supported");
    }

    switch (algo) {
        case 0:
            return AxbAlgorithm::PointJacobi;
        case 1:
            return AxbAlgorithm::GaussSeidel;
        case 2:
            return AxbAlgorithm::SuccessiveOverRelaxation;
        case 3:
            return AxbAlgorithm::LUP;
        default:
            throw std::runtime_error("Invalid algorithm code");
    }
}

#endif // LINALG_AXB_IO_H
