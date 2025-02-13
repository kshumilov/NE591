#ifndef LINALG_OPS_H
#define LINALG_OPS_H

#include <concepts>
#include <vector>

#include <fmt/core.h>

#include "methods/linalg/blas.h"


template<typename T>
concept Sizeable = requires(T v) {
     v.size();
};

template<Sizeable T, Sizeable U>
constexpr bool same_size(T t, U u) noexcept
{
    return t.size() == u.size();
}


template<std::floating_point scalar_t>
std::vector<scalar_t>& operator-=(std::vector<scalar_t>& lhs, const std::vector<scalar_t>& rhs)
{
    #ifndef NDEBUG
    if (not same_size(lhs, rhs))
        throw std::invalid_argument(fmt::format(
            "lhs and rhs must be the same size: {} != {}", lhs.size(), rhs.size()
        ));
    #endif

    axpy<scalar_t>(rhs, lhs, scalar_t{-1});
    return lhs;
}


template<std::floating_point scalar_t>
std::vector<scalar_t>& operator+=(std::vector<scalar_t>& lhs, const std::vector<scalar_t>& rhs)
{
    #ifndef NDEBUG
    if (not same_size(lhs, rhs))
        throw std::invalid_argument(fmt::format(
            "lhs and rhs must be the same size: {} != {}", lhs.size(), rhs.size()
        ));
    #endif

    axpy<scalar_t>(rhs, lhs);
    return lhs;
}


template<std::floating_point scalar_t>
std::vector<scalar_t>& operator*=(std::vector<scalar_t>& lhs, const scalar_t val)
{
    scal(lhs, val);
    return lhs;
}


template<std::floating_point scalar_t>
std::vector<scalar_t> operator*(std::vector<scalar_t> lhs, const scalar_t val)
{
    lhs *= val;
    return lhs;
}


template<std::floating_point scalar_t>
std::vector<scalar_t> operator/(std::vector<scalar_t> lhs, const scalar_t val)
{
    lhs *= scalar_t{1} / val;
    return lhs;
}


template<std::floating_point scalar_t>
auto operator/=(std::vector<scalar_t>& lhs, const scalar_t val) -> std::vector<scalar_t>&
{
    return operator*=(lhs, scalar_t{1} / val);
}


template<std::floating_point scalar_t>
auto operator+(const std::vector<scalar_t>& lhs, const std::vector<scalar_t>& rhs) -> std::vector<scalar_t>
{
    #ifndef NDEBUG
    if (not same_size(lhs, rhs))
        throw std::invalid_argument(fmt::format(
            "lhs and rhs must be the same size: {} != {}", lhs.size(), rhs.size()
        ));
    #endif

    std::vector<scalar_t> result{lhs};
    result += rhs;
    return result;
}

template<std::floating_point scalar_t>
auto operator-(const std::vector<scalar_t>& lhs, const std::vector<scalar_t>& rhs) -> std::vector<scalar_t>
{
    #ifndef NDEBUG
    if (not same_size(lhs, rhs))
        throw std::invalid_argument(fmt::format(
            "lhs and rhs must be the same size: {} != {}", lhs.size(), rhs.size()
        ));
    #endif

    std::vector<scalar_t> result{lhs};
    result -= rhs;
    return result;
}

#endif // LINALG_OPS_H
