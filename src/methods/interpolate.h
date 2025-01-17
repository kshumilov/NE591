#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include <cstddef>
#include <concepts>
#include <ranges>
#include <stdexcept>
#include <vector>
#include <bits/ranges_algo.h>

template<std::floating_point T>
class LagrangeInterpolation {
    using data = std::span<const T>;

    std::span<const T> m_x{};  // Interpolated points
    std::span<const T> m_y{};  // y = f(x);

    auto Lj(const std::size_t j, const T x) const -> double {
        // if (xs.size() <= k) {
        //     throw std::logic_error("k is not in the interval [0, xs.size() - 1]");
        // }

        T result { 1.0 };
        for (const auto i: std::ranges::iota(0U, m_x.size())) {
            if (i != j)
                result *= (x - m_x[i]) / (m_x[j] - m_x[i]);
        }
        return result;
    }

public:
    LagrangeInterpolation(const std::span<const T>& xi, const std::span<const T>& yi)
    : m_x{xi}, m_y{yi}
    {
        if (xi.empty()) {
            throw std::invalid_argument("xi is empty");
        }

        if (xi.size() != yi.size()) {
            throw std::logic_error("xi.size() != yi.size()");
        }

        if (not std::ranges::is_sorted(xi)) {
            throw std::logic_error("xi is not sorted");
        }
    }

    auto operator()(const T x) const -> T {
        // if (x < m_x[0U] || m_x[m_x.size() - 1U] < x) {
        //     throw std::logic_error("x is not in the interval [x[0], x[-1]]");
        // }

        T result { 0.0 };

        for (const auto j: std::ranges::iota(0, m_x.size())) {
            result += m_y[j] * Lk(j, x);
        }

        return result;
    }

    auto operator()(const std::span<const T> xs) const -> std::vector<T> {
        auto rg = xs | std::views::transform(*this);
#ifdef __cpp_lib_containers_ranges
        return { std::from_range, rg };
#else
        return { rg.begin(), rg.end() };
#endif
    }
};

#endif //INTERPOLATE_H
