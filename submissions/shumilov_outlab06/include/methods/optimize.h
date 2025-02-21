#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <istream>
#include <concepts>
#include <limits>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#ifndef NDEBUG
#include <fmt/ostream.h>
#endif

#include "methods/utils/io.h"


template<std::floating_point DType = long double>
struct FixedPointIterSettings {
    DType tolerance{1.0e-8};
    int max_iter{100};

    FixedPointIterSettings() = default;
    FixedPointIterSettings(const FixedPointIterSettings&) = default;
    FixedPointIterSettings(FixedPointIterSettings&&) = default;
    FixedPointIterSettings& operator=(const FixedPointIterSettings&) = default;
    FixedPointIterSettings& operator=(FixedPointIterSettings&&) = default;

    [[nodiscard]] constexpr
    FixedPointIterSettings(const DType tolerance_, const int max_iter_)
        : tolerance(tolerance_)
        , max_iter(max_iter_)
    {
        if (max_iter <= int{}) {
            throw std::invalid_argument(fmt::format(
                "`max_iter` must be positive: {: d}", max_iter
            ));
        }

        if (tolerance <= DType{}) {
            throw std::invalid_argument(fmt::format(
                "`tolerance` must be positive: {: 12.6e}", tolerance
            ));
        }
    }

    [[nodiscard]] constexpr
    auto operator==(const FixedPointIterSettings& other) const -> bool {
        return max_iter == other.tolerance and isclose(tolerance, other.tolerance);
    }

    [[nodiscard]] constexpr
    auto to_string() const -> std::string {
        return fmt::format(
            "Fixed-Point Iteration:\n"
            "\tMaximum Number of Iterations: {:d}\n"
            "\tTolerance: {:12.6e}",
            max_iter, tolerance
        );
    }

    [[nodiscard]]
    static auto from_file(std::istream &input) -> FixedPointIterSettings {
        return FixedPointIterSettings{
            read_positive_value<DType>(input, "tolerance"),
            read_positive_value<int>(input, "max_iter"),
        };
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FixedPointIterSettings<DType>, tolerance, max_iter)
};


template<class T, std::floating_point DType = long double>
struct FixedPointIterResult {
    T x{};
    bool converged{false};
    int iters{};
    DType error{std::numeric_limits<DType>::infinity()};

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
 * @param error A function that determines the error based on current and previous points
 * @param settings Fixed Point Iteration Setting
 *
 * @return A pair containing the estimated root and a boolean indicating convergence.
 */
template<class T, std::invocable<T> G, std::invocable<T, T> DeltaError, std::floating_point DType = long double>
constexpr auto fixed_point_iteration(
    G g, const T& x0, DeltaError error,
    const FixedPointIterSettings<DType> settings
) -> FixedPointIterResult<T, DType>
{
    #ifndef NDEBUG
    fmt::println(
        std::cerr,
        "{:s}\n"
        "Starting Point:\n"
        "{}",
        settings.to_string(), x0
    );
    #endif

    auto x_curr = x0;
    auto current_error = std::numeric_limits<DType>::infinity();

    for (int i = 0; i < settings.max_iter; ++i) {
        const auto x_next = g(x_curr);
        current_error = error(x_next, x_curr);

        #ifndef NDEBUG
        fmt::println(
            std::cerr,
            "#{: >3d}/{: >3d}: {:12.6e}\n"
            "x_next: {}\n"
            "x_curr: {}\n",
            i + 1, settings.max_iter, current_error, x_next, x_curr
        );
        #endif

        if (current_error < settings.tolerance) {
            return FixedPointIterResult<T, DType>{
                .x = x_next,
                .converged = true,
                .iters = i + 1 ,
                .error = current_error
            };
        }

        x_curr = x_next;
    }

    return FixedPointIterResult<T, DType>{
        .x = x_curr,
        .converged = false,
        .iters = settings.max_iter,
        .error = current_error
    };
}


template<class T, std::invocable<T> G, std::invocable<T> AbsError, std::floating_point DType = long double>
constexpr auto fixed_point_iteration(
    G g, const T& x0, AbsError error,
    const FixedPointIterSettings<DType> settings
) -> FixedPointIterResult<T, DType>
{
    #ifndef NDEBUG
    fmt::println(
        std::cerr,
        "{:s}\n"
        "Starting Point:\n"
        "{}",
        settings.to_string(), x0
    );
    #endif

    auto x = x0;
    auto current_error = std::numeric_limits<DType>::infinity();

    for (int i = 0; i < settings.max_iter; ++i) {
        x = g(x);
        current_error = error(x);

        #ifndef NDEBUG
        fmt::println(
            std::cerr,
            "#{: >3d}/{: >3d}: {:12.6e}\n"
            "x: {}\n",
            i + 1, settings.max_iter, current_error, x
        );
        #endif

        if (current_error < settings.tolerance) {
            return FixedPointIterResult<T, DType>{
                .x = x,
                .converged = true,
                .iters = i + 1 ,
                .error = current_error
            };
        }
    }

    return FixedPointIterResult<T, DType>{
        .x = x,
        .converged = false,
        .iters = settings.max_iter,
        .error = current_error
    };
}
#endif // OPTIMIZE_H
