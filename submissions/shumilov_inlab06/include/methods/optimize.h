#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <iostream>
#include <concepts>
#include <utility>  // pair
#include <limits>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include "methods/utils/io.h"


struct FixedPointIterSettings {
    int max_iter{100};
    long double tolerance{1.0e-8};

    [[nodiscard]]
    constexpr auto to_string() const -> std::string {
        return fmt::format("Fixed-Point Iteration (max_iter={:d}, tol={:.2e})", max_iter, tolerance);
    }
};

template<class T>
struct FixedPointIterResult {
    T x{};
    bool converged{false};
    int iters{};
    long double error{};

    [[nodiscard]]
    constexpr auto to_string() const -> std::string {
        if (converged) {
            return fmt::format("Converged at iteration #{:d}: {:12.6e}", iters, error);
        }
        return fmt::format("Failed to converge in {:d} iterations: {:12.6e}", iters, error);
    }
};


/**
 * @brief Performs fixed-point iteration to find the root of a function.
 *
 * This function is generalized to accept a custom update function `g` and
 * a convergence criterion function `converged`.
 *
 * @tparam T The type of the input and output, typically `double`.
 *
 * @param g The update function for the iteration.
 * @param x0 The initial guess for the root.
 * @param converged A function that determines if the iteration has converged.
 * @param max_iter The maximum number of iterations.
 *
 * @return A pair containing the estimated root and a boolean indicating convergence.
 */
template<class T, std::invocable<T> G, std::invocable<T, T> C>
constexpr auto fixed_point_iteration(G g, const T& x0, C converged, const int max_iter) -> std::pair<T, bool>
{
    auto x = x0;
    for (int i = 0; i < max_iter; ++i) {
        auto x_next = g(x);
        if (converged(x_next, x)) {
            return {x_next, true};
        }
        x = x_next;
    }
    return {x, false};
}


/**
 * @brief Performs fixed-point iteration to find the root of a function.
 *
 * This function is generalized to accept a custom update function `g` and
 * a convergence criterion function `converged`.
 *
 * @tparam T The type of the input and output, typically `double`.
 *
 * @param g The update function for the iteration.
 * @param x0 The initial guess for the root.
 * @param error A function that determines the error based on current and previous points
 * @param settings
 *
 * @return A pair containing the estimated root and a boolean indicating convergence.
 */
template<class T, std::invocable<T> G, std::invocable<T, T> C>
constexpr auto fixed_point_iteration(
    G g, const T& x0, C error,
    const FixedPointIterSettings& settings) -> FixedPointIterResult<T>
{
//    #ifndef NDEBUG
//    fmt::println(std::cerr, "{:s}", settings.to_string());
//    fmt::println(std::cerr, "Starting point:");
//    fmt::println(std::cerr, "{}", x0);
//    #endif

    auto x = x0;
    long double curr_err = std::numeric_limits<long double>::infinity();

    for (int i = 0; i < settings.max_iter; ++i) {
        auto x_next = g(x);
        curr_err = error(x_next, x);

//        #ifndef NDEBUG
//        fmt::println(std::cerr, "#{: >3d}/{: >3d}: {:12.6e}", i + 1, settings.max_iter, curr_err);
//        fmt::println(std::cerr, "\tx_next: {::12.6e}", x_next);
//        fmt::println(std::cerr, "\tx_curr: {::12.6e}", x);
//        #endif

        if (curr_err < settings.tolerance) {
            return FixedPointIterResult<T>{
                .x = x_next
              , .converged = true
              , .iters = i + 1
              , .error = curr_err
            };
        }
        x = x_next;
    }

    return FixedPointIterResult<T>{
        .x = x
      , .converged = false
      , .iters = settings.max_iter
      , .error = curr_err
    };
}
#endif // OPTIMIZE_H
