#ifndef LINALG_AXB_GS_H
#define LINALG_AXB_GS_H


#include "methods/linalg/Axb/sor.h"


template<std::floating_point DType, std::invocable<std::size_t, std::size_t> MatElem>
constexpr auto gauss_seidel
(
        MatElem A,
        std::span<const DType> b,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
        return successive_over_relaxation<DType>(A, b, 1.0, settings);
}


template<std::floating_point DType>
constexpr auto gauss_seidel(
        const Matrix<DType>& A,
        std::span<const DType> b,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
        return successive_over_relaxation<DType>(A, b, 1.0, settings);
}


template<std::floating_point DType>
constexpr auto gauss_seidel(
        const std::pair<Matrix<DType>, std::vector<DType>> &linear_system,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}) -> IterativeAxbResult<DType> {
    return gauss_seidel<DType>(linear_system.first, linear_system.second, settings);
}

#endif // LINALG_AXB_GS_H
