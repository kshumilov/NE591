#ifndef ARRAY_H
#define ARRAY_H

#include <vector>
#include <concepts>
#include <ranges>

/**
 *
 * \brief Constructes a vector of equidistant #num points in the interval [start, end] inclusively
 *
 * @tparam T Floating point type, can be float, double, long double
 * @param start left side of the interval, real
 * @param end right sie of the interval, real
 * @param num number of points in the interval
 * @return vector of equidistant points in [start,end]
 */
template<std::floating_point T>
auto linspace(const T start, const T end, const int num) -> std::vector<T> {
    if (num == 0) {
        return {start};
    }

    T step { (end - start) / static_cast<T>(num - 1) };

    auto rg = std::views::iota(0, num)
            | std::views::transform([&](auto i) { return start + i * step; });

    return { rg.cbegin(), rg.cend() };
}


#endif //ARRAY_H
