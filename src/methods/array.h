#ifndef ARRAY_H
#define ARRAY_H

#include <vector>
#include <concepts>
#include <ranges>


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
