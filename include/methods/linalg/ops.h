//
// Created by Kirill Shumilov on 2/12/25.
//

#ifndef OPS_H
#define OPS_H

#include <concepts>
#include <span>
#include <numeric>
#include <cmath>
#include <cassert>

template<std::floating_point scalar_t>
auto operator-=(std::vector<scalar_t>& lhs, const std::vector<scalar_t>& rhs) -> std::vector<scalar_t>
{
    assert(lhs.size() == rhs.size());
    for (std::size_t i = 0; i < lhs.size(); i++) {
        lhs[i] -= rhs[i];
    }
    return lhs;
}


template<std::floating_point scalar_t>
auto operator+=(std::vector<scalar_t>& lhs, std::span<const scalar_t> rhs) -> std::span<scalar_t>
{
    assert(lhs.size() == rhs.size());
    for (std::size_t i = 0; i < lhs.size(); i++) {
        lhs[i] += rhs[i];
    }
    return lhs;
}


template<std::floating_point scalar_t>
auto operator-(const std::vector<scalar_t>& lhs, const std::vector<scalar_t>& rhs) -> std::vector<scalar_t>
{
    assert(lhs.size() == rhs.size());

    std::vector<scalar_t> result{lhs.begin(), lhs.end()};
    result -= rhs;

    return result;
}

template<std::floating_point scalar_t>
auto norm_l2(std::span<const scalar_t> v) -> scalar_t
{
    return std::sqrt(std::transform_reduce(
        v.cbegin(), v.cend(),
        scalar_t{}, std::plus<scalar_t>{},
        [](const auto vi) constexpr {
            return vi * vi;
        }
    ));
}
#endif //OPS_H
