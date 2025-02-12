#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <concepts>
#include <utility>  // pair


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

#endif // OPTIMIZE_H
