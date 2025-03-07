#ifndef LEGENDRE_H
#define LEGENDRE_H

#include <cstddef>
#include <concepts>
#include <numbers>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ranges>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include "linalg/blas.h"
#include "methods/roots.h"
#include "methods/utils/math.h"


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
    assert(-scalar_t{1} <= x && x <= scalar_t{1});
    assert(l >= 0);

    if (l == 0) {
        return scalar_t{1};
    }

    if (l == 1) {
        return x;
    }

    scalar_t Pl = 0;
    scalar_t Plmin1 = x;
    scalar_t Plmin2 = 1.;

    for (int k = 2; k <= l; ++k)
    {
        const auto k_{static_cast<size_t>(k)};
        Pl = ((2 * k_ - 1) * x * Plmin1 - (k_ - 1) * Plmin2) / k_;
        Plmin2 = Plmin1;
        Plmin1 = Pl;
    }

    return Pl;
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
constexpr auto legendre_root(const int l, const int k, const FixedPointIterSettings<scalar_t>& settings) -> FixedPointIterResult<scalar_t, scalar_t>
{
    const auto x0{guess_legendre_root<scalar_t>(l, k)};

    auto f = [l](const scalar_t x) constexpr -> scalar_t {
        return legendre<scalar_t>(x, l);
    };

    auto df = [l](const scalar_t x) constexpr -> scalar_t {
        const auto value = static_cast<scalar_t>(l) / (x * x - scalar_t{1}) * (x * legendre(x, l) - legendre(x, l - 1));
        return value;
    };
    #ifndef NDEBUG
    fmt::println(std::cerr, "Finding Legendre Root of ({}, {})", l, k);
    #endif
    return newton_raphson<scalar_t>(f, df, x0, settings);
}

template<std::floating_point scalar_t>
auto legendre_roots(const int l, const int k_min, const int k_max, const FixedPointIterSettings<scalar_t>& settings) -> std::vector<scalar_t>
{
    assert(1 <= k_min && k_min < k_max && k_max <= l);
    return std::ranges::to<std::vector>(
        std::views::iota(k_min, k_max + 1) | std::views::transform(
            [&](const int k) -> scalar_t
            {
                const auto result = legendre_root<scalar_t>(l, k, settings);
                if (not result.converged)
                {
                    throw std::runtime_error(fmt::format("Could not converge Legendre root l = {}, k = {}", l, k));
                }
                return result.x;
            }
        )
    );
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
auto legendre_roots(const int l, const FixedPointIterSettings<scalar_t>& settings) -> std::vector<scalar_t>
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
        const auto result = legendre_root<scalar_t>(l, k, settings);
        if (not result.converged)
        {
            throw std::runtime_error(fmt::format("Could not converge Legendre root l = {}, k = {}", l, k));
        }
        roots.emplace_back(result.x);
        roots.emplace_back(-result.x);
        k++;
    }

    std::sort(roots.begin(), roots.end());

    return roots;
}


/**
 * @brief Calculate Quadrature weights for Legendre Polynomial of degree l
 *
 * @tparam scalar_t type of scalar used, can be `float`, `double`, or `long double`
 *
 * @param l Degree of legendre polynomial
 * @param x k'th root for Legendre Polynomial of degree l
 *
 * @return Quadrature weight for x'th node
 */
template<std::floating_point scalar_t>
constexpr auto legendre_weight(const int l, const scalar_t x) -> scalar_t
{
    assert(l >= 2);
    const scalar_t num {2.0 * (1.0 - x * x)};
    const scalar_t denom_sqrt {static_cast<scalar_t>(l + 1) * legendre(x, l + 1)};
    return num / (denom_sqrt * denom_sqrt);
}


template<std::floating_point scalar_t>
constexpr auto gauss_legendre_quadrature(
    const int l,
    const FixedPointIterSettings<scalar_t> settings
) -> std::pair<std::vector<scalar_t>, std::vector<scalar_t>>
{
    const auto nodes = legendre_roots<scalar_t>(l, settings);
    std::vector<scalar_t> weights(nodes.size());

    std::transform(
        nodes.cbegin(), nodes.cend(),
        weights.begin(),
        [&l](const scalar_t x) constexpr -> scalar_t {
            return legendre_weight<scalar_t>(l, x);
        }
    );

    #ifndef NDEBUG
    const auto weight_sum = std::accumulate(weights.cbegin(), weights.cend(), scalar_t{0.0});
    assert(isclose<scalar_t>(weight_sum, scalar_t{2}));
    #endif

    return std::make_pair(std::move(nodes), std::move(weights));
}

template<std::floating_point scalar_t>
constexpr auto gauss_legendre_quadrature(
    const int l, const int k_min, const int k_max,
    const FixedPointIterSettings<scalar_t> settings
) -> std::pair<std::vector<scalar_t>, std::vector<scalar_t>>
{
    const auto nodes = legendre_roots<scalar_t>(l, k_min, k_max, settings);
    std::vector<scalar_t> weights(nodes.size());

    std::transform(
        nodes.cbegin(), nodes.cend(),
        weights.begin(),
        [&l](const scalar_t x) constexpr -> scalar_t {
            return legendre_weight<scalar_t>(l, x);
        }
    );

    return std::make_pair(std::move(nodes), std::move(weights));
}


template<std::floating_point scalar_t>
auto print_gauss_legendre_quadrature(
    const std::pair<std::vector<scalar_t>, std::vector<scalar_t>>& quad,
    std::ostream& out = std::cout
) -> void
{
    const auto&[nodes, weights] = quad;
    const auto sum = std::accumulate(weights.cbegin(), weights.cend(), scalar_t{0.0});

    std::string result{};
    fmt::format_to(
        std::back_inserter(result),
        "{:^{}}\n",
        fmt::format("{:^5} {:^24} {:^24}", "i", "nodes", "weights"),
        80
    );

    for (const auto [i, x, w] : std::views::zip(std::views::iota(1U), nodes, weights))
        fmt::format_to(
            std::back_inserter(result),
            "{:^{}}\n",
            fmt::format("{:<5d} {:> 24.16e} {:> 24.16e}", i, x, w),
            80
        );

    fmt::print(
        out,
        "{2:^{1}}\n"
        "{0:-^{1}}\n"
        "{3}"
        "{0:-^{1}}\n"
        "{4: ^{1}s}",
        "",
        80,
        fmt::format("Gauss Quadrature, I = {:d}", nodes.size()),
        result,
        fmt::format("sum(weights) = {:^24.16e}", sum)
    );

}

#endif //LEGENDRE_H