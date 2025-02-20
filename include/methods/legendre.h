#ifndef LEGENDRE_H
#define LEGENDRE_H

#include <cstddef>
#include <concepts>
#include <numbers>
#include <cassert>
#include <cmath>
#include <vector>

#include "methods/roots.h"


/**
 * @brief Evaluate Legendre polynomial of degree l using recurrence relation
 *
 * @details l * P_l(x) = (2l - 1) * P_{l-1}(x) - (l - 1) * P_{l-2}(x);
 *          P0(x) = 1.0;
 *          P1(x) = x;
 *
 * @tparam scalar_t type of scalar used, can be `float`, `double`, or `long double`
 * @param x point to evaluate Pl(x) at
 * @param l degree of Legendre Polynomial
 *
 * @return Pl(x)
 */
template<std::floating_point scalar_t>
constexpr auto legendre(const scalar_t x, const int l) -> scalar_t
{
    assert(-1. <= x && x <= 1.);
    assert(l >= 0);

    if (l == 0) {
        return 1.0;
    }

    if (l == 1) {
        return x;
    }

    const auto l_ = static_cast<scalar_t>(l);
    return ((2.0 * l_ - 1.0) * x * legendre(x, l - 1U) - (l_ - 1.0) * legendre(x, l - 2U)) / l_;
}


/**
 * @brief Calculate an approximation to kth root of Legendre Polynomial of degree l
 *
 * @link https://naturalunits.blogspot.com/2013/10/zeros-of-legendre-polynomials.html
 * @link https://doi.org/10.1007/BF02428258
 *
 * @tparam scalar_t type of scalar used, can be `float`, `double`, or `long double`
 *
 * @param l degree of Legendre Polynomial, 1 <= l
 * @param k index of the root in the ascent order, 1 <= k <= l
 *
 * @return Approximation to the kth root of P_l(x)
 */
template<std::floating_point scalar_t>
constexpr auto guess_legendre_root(const int l, const int k) -> scalar_t
{
    assert(1 <= l);  // No roots for P0(x) = 1
    assert(1 <= k && k <= l);

    const auto l_{static_cast<scalar_t>(l)};
    const auto k_{static_cast<scalar_t>(k)};

    const auto l_sq{l_ * l_};
    const auto theta_lk{(l_ - k_ + 0.75) / (l_ + 0.5) * std::numbers::pi_v<scalar_t>};
    const auto sin_theta_lk = std::sin(theta_lk);

    return std::cos(theta_lk) * (
        1.0 -
        1.0 / (8.0 * l_sq) * (
            1.0 - 1.0 / l_
                + 1.0 / (48.0 * l_sq * l_sq) * (39.0 - 28.0 / (sin_theta_lk * sin_theta_lk))
        )
    );
}


/**
 * @brief Refine guess for Legendre Root using Newton-Raphson root-finding algorithm
 *
 * @link https://naturalunits.blogspot.com/2013/10/zeros-of-legendre-polynomials.html
 * @link https://doi.org/10.1007/BF02428258
 *
 * @tparam scalar_t type of scalar used, can be `float`, `double`, or `long double`
 *
 * @param l degree of Legendre Polynomial
 * @param k index of the root in the ascent order, k <= l
 *
 * @param tol The tolerance of Newton-Raphson convergence
 * @param max_iter The maximum number of Newton-Raphson Iterations
 *
 * @return Approximation to the kth root of P_l(x)
 */
template<std::floating_point scalar_t>
constexpr auto legendre_root(const int l, const int k, const scalar_t tol = 1e-8, const int max_iter = 5) -> std::pair<scalar_t, bool>
{
    const auto x0{guess_legendre_root<scalar_t>(l, k)};

    auto f = [l](const scalar_t x) constexpr -> scalar_t {
        return legendre<scalar_t>(x, l);
    };

    auto df = [l](const scalar_t x) constexpr -> scalar_t {
        return static_cast<scalar_t>(l) / (x * x - 1.0) * (x * legendre(x, l) - legendre(x, l - 1));
    };

    return newton_raphson<scalar_t>(f, df, x0, {tol, max_iter});
}



/**
 * @brief Find all roots of Legendre polynomial for a given degree
 *
 * @link https://naturalunits.blogspot.com/2013/10/zeros-of-legendre-polynomials.html
 * @link https://doi.org/10.1007/BF02428258
 *
 * @tparam scalar_t type of scalar used, can be `float`, `double`, or `long double`
 *
 * @param l degree of Legendre Polynomial
 *
 * @return Vector of unique xi such that Pl(xi)=0
 */
template<std::floating_point scalar_t>
auto legendre_roots(const int l) -> std::vector<scalar_t>
{
    assert(l >= 0);

    std::vector<scalar_t> roots{};

    // P0(x) = 1.0 - no nodes
    if (l == 0)
        return roots;

    // Nodes are symmetric about x = 0, so only compute nodes in (0,1)
    auto k {l / 2 + 1};

    // if n is odd, add root at 0.0
    if (l % 2) {
        roots.emplace_back(0.0);
        k++;
    }

    // now add nodes until we get all n
    while (roots.size() < static_cast<std::size_t>(l)) {
        const auto [root, converged] = legendre_root<scalar_t>(l, k);
        roots.emplace_back(root);
        roots.emplace_back(-root);
        k++;
    }

    std::sort(roots.begin(), roots.end());

    return roots;
}
#endif //LEGENDRE_H