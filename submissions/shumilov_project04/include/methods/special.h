#ifndef SPECIAL_H
#define SPECIAL_H

#include <concepts>
#include <cmath>

template<std::floating_point T>
struct Gaussian2D {
    T x0{}, y0{};
    T fwhm{1.0};
    T amplitude{1.0};

    constexpr auto operator()(std::pair<T, T>&& point)
    {
        return this->operator()(point.first, point.second);
    }

    constexpr auto operator()(const T x, const T y) const
    {
        const auto two_sigma_sq = 2 * sigma() * sigma();
        const auto N = amplitude / (std::numbers::pi * two_sigma_sq);
        return N * exp(-((x - x0) * (x - x0) + (y - y0) * (y - y0)) / two_sigma_sq);
    }

    constexpr auto sigma() const
    {
        return fwhm / (2 * sqrt(2 * log(2)));
    }
};

#endif //SPECIAL_H
