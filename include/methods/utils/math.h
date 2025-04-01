#ifndef UTILS_MATH_H
#define UTILS_MATH_H

#include <cmath>
#include <concepts>

enum class IsClosePolicy
{
    Symmetric,
    Relative,
};


template<std::floating_point T, IsClosePolicy policy = IsClosePolicy::Relative>
struct IsClose
{
    static constexpr T default_rel_dol {1.e-5};
    static constexpr T default_abs_tol {1.e-5};

    T rel_tol{ default_rel_dol };
    T abs_tol{ default_abs_tol };

    [[nodiscard]]
    constexpr auto operator()(const T a, const T b) const noexcept
    {
        if constexpr (policy == IsClosePolicy::Relative) {
            return std::abs(a - b) <= abs_tol + rel_tol * std::abs(b);
        }
        else
        {
            return std::abs(a - b) <= std::max(rel_tol * std::max(std::abs(a), std::abs(b)), abs_tol);
        }
    }

    [[nodiscard]]
    constexpr auto operator()(const T a) const noexcept
    {
        return this->operator()(a, T{});
    }
};


template<std::floating_point scalar_t, IsClosePolicy policy = IsClosePolicy::Relative>
constexpr auto isclose
(
    const scalar_t a,
    const scalar_t b,
    const scalar_t rtol = scalar_t{ IsClose<scalar_t>::default_rel_dol },
    const scalar_t atol = scalar_t{ IsClose<scalar_t>::default_abs_tol }
) -> bool
{
    // return std::abs(a - b) <= atol + rtol * std::abs(b);
    return IsClose<scalar_t, policy>(rtol, atol)(a, b);
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
