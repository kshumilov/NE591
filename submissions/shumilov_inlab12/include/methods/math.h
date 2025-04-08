#ifndef UTILS_MATH_H
#define UTILS_MATH_H

#include <cmath>
#include <concepts>

template<std::floating_point scalar_t>
constexpr auto isclose(const scalar_t a, const scalar_t b, const scalar_t rtol = scalar_t{1.0e-05}, const scalar_t atol = scalar_t{1.0e-08}) -> bool
{
   return std::abs(a - b) <= atol + rtol * std::abs(b);
}

template<std::floating_point U, std::floating_point V>
constexpr auto abs_diff(const U test, const V ref)
{
    return std::abs(test - ref);
}

template<std::floating_point U, std::floating_point V>
constexpr auto rel_diff(const U test, const V ref)
{
    using value_type = std::common_type_t<U, V>;
    const auto zero = value_type{0};
    const auto one = value_type{1};
    if (test == zero and isclose(ref, zero))
        return zero;
    return std::abs(test / ref - one);
}

template<std::floating_point U, std::floating_point V>
constexpr auto rel_err(const U err, const V val)
{
    using value_type = std::common_type_t<U, V>;
    const auto zero = value_type{0};
    if (val == zero)
        return std::numeric_limits<value_type>::infinity();
    return std::abs(err / val);
}

#endif //UTILS_MATH_H
