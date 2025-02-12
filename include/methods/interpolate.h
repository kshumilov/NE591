#ifndef INTERPOLATE_H
#define INTERPOLATE_H


#include <cstddef>
#include <concepts>
#include <ranges>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "methods/array.h"

/**
 * \brief Class that implement Lagrange polynomial interpolation
 *
 * \detail This class conforms to struct-of-arrays format of storing data.
 *         It's based on the non-owning views (`std::span`) of the data.
 *
 * \tparam scalar_t type of data points, can be `float`, `double`, or `long double`
 */
template<std::floating_point scalar_t>
class LagrangeInterpolation {
    std::span<const scalar_t> m_x{};  /** x, interpolated values, must be sorted in strictly increasing order */
    std::span<const scalar_t> m_y{};  /** values of interpolated function, y_i = f(x_i) */

public:
    /**
     * \brief Constructor to build LagrangeInterpolation
     *
     * @param x sequence of interpolated points, \f${x}_i\f$
     * @param y sequence of values at interpolated points, \f$y_i=f(x_i)\f$, must equal in size to \p x
     *
     * \throws std::invalid_argument x is empty
     * \throws std::logic_error size(x) != size(y)
     * \throws std::logic_error x is not sorted
     */
    constexpr LagrangeInterpolation(std::span<const scalar_t> x, std::span<const scalar_t> y)
        : m_x{x}
        , m_y{y}
    {
        if (x.empty()) {
            throw std::invalid_argument("x is empty");
        }

        if (x.size() != y.size()) {
            throw std::logic_error("x.size() != y.size()");
        }
    }

    /**
     * \brief a function to evaluate kth Lagrange Polynomial at a point [min(x), max(x)]
     *
     * \f[
     * L_k(x) = \prod^n_{i=0,i\neq k}\frac{x - x_i}{x_k - x_i}
     * \f]
     *
     * \warning: bound checks are not performed on x, use at your discretion
     *
     * \param[in] k index of the Lagrange polynomial
     * \param[in] x value to evaluate \f$L_k\f$ at.
     *
     * \return \f$L_k(x)\f$
     */
    constexpr scalar_t L(const std::size_t k, const scalar_t x) const
    {
        scalar_t result{1.0};

        auto f = [&](const std::size_t i) {
            return (x - m_x[i]) / (m_x[k] - m_x[i]);
        };

        // i < k
        for (const auto i: std::views::iota(0U, k)) {
            result *= f(i);
        }

        // i == k -- skip

        // i > k
        for (const auto i: std::views::iota(k + 1U, m_x.size())) {
            result *= f(i);
        }

        return result;
    }

    /**
     * \brief Evaluates Lagrange Interpolation Polynomial, \f$p_n(x)\f$
     *
     * \param x value to evaluate Lagrange Interpolation Polynomial
     *
     * \return \f$p_n(x)\f$
     */
    constexpr scalar_t operator()(const scalar_t x) const
    {
        auto rg = std::views::zip_transform(
            [&](auto y_k, auto k) {
                return y_k * L(k, x);
            },
            m_y, std::views::iota(0U, m_y.size())
        );

        return std::accumulate(rg.cbegin(), rg.cend(), scalar_t{});
    }

    /**
     * \brief operator() overload for sequence of input values
     *
     * @param x sequence of values to evaluate Lagrange Interpolation Polynomial at
     *
     * @return vector of evaluated \f$p_n(x)\f$
     */
    constexpr std::vector<scalar_t> operator()(const std::span<const scalar_t> x) const
    {
        auto rg = x | std::views::transform(*this);
        return {rg.cbegin(), rg.cend()};
    }

    /**
     * \brief Sample LIP in [min(x), max(x)]
     *
     * @param num number of equidistant points in the interval [min(x), max(x)] to evaluate LIP
     *            x(0) and x(-1) are included in the interpolation points
     *
     * @return pair of vectors {x, p_n(x)}
     */
    constexpr auto sample(const int num) const -> std::pair<std::vector<scalar_t>, std::vector<scalar_t> >
    {
        const auto [min_x, max_x] = std::minmax_element(m_x.cbegin(), m_x.cend());
        const auto x = linspace(*min_x, *max_x, num);
        return {x, this->operator()(x)};
    }
};

#endif //INTERPOLATE_H