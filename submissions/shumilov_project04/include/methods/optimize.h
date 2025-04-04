#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <istream>
#include <limits>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#ifndef NDEBUG
#include <fmt/ostream.h>
#endif

#include "methods/utils/io.h"

enum class ParamOrder
{
    ToleranceFirst, MaxIterFirst,
};

template<std::floating_point ErrorType = long double>
struct FixedPointIterSettings
{
    ErrorType tolerance{ 1.0e-8 };
    int max_iter{ 100 };

    constexpr FixedPointIterSettings() = default;

    constexpr FixedPointIterSettings(const FixedPointIterSettings&) = default;

    constexpr FixedPointIterSettings(FixedPointIterSettings&&) = default;

    constexpr FixedPointIterSettings& operator=(const FixedPointIterSettings&) = default;

    constexpr FixedPointIterSettings& operator=(FixedPointIterSettings&&) = default;


    [[nodiscard]]
    constexpr FixedPointIterSettings(const ErrorType tolerance_, const int max_iter_)
        : tolerance(tolerance_)
      , max_iter(max_iter_)
    {
        if (max_iter <= int{})
        {
            throw std::invalid_argument(fmt::format("`max_iter` must be positive: {: d}", max_iter));
        }

        if (tolerance <= ErrorType{})
        {
            throw std::invalid_argument(fmt::format("`tolerance` must be positive: {: 12.6e}", tolerance));
        }
    }


    [[nodiscard]]
    constexpr auto operator==(const FixedPointIterSettings& other) const
    {
        return max_iter == other.tolerance and isclose(tolerance, other.tolerance);
    }

    [[nodiscard]] constexpr auto to_string(const int label_width = 40) const {
        return fmt::format(
                "Fixed-Point Iteration:\n"
                "\t{1:.<{0}s}: {2:d}\n"
                "\t{3:.<{0}s}: {4:12.6e}",
                label_width,
                "Maximum Number of Iterations",
                max_iter,
                "Tolerance",
                tolerance);
    }


    template<ParamOrder Order = ParamOrder::ToleranceFirst>
    [[nodiscard]]
    static auto from_file(std::istream& input) -> FixedPointIterSettings
    {
        if constexpr (Order == ParamOrder::ToleranceFirst)
        {
            return FixedPointIterSettings{
                read_positive_value<ErrorType>(input, "tolerance"),
                read_positive_value<int>(input, "max_iter"),
            };
        }
        else
        {
            const auto max_iter = read_positive_value<int>(input, "max_iter");
            const auto tolerance = read_positive_value<ErrorType>(input, "tolerance");
            return FixedPointIterSettings{
                tolerance,
                max_iter,
            };
        }
    }


    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FixedPointIterSettings<ErrorType>, tolerance, max_iter)
};


template<class T, std::floating_point ErrorType = long double>
struct FixedPointIterResult
{
    T x{};
    bool converged{ false };
    int iters{};
    ErrorType error{ std::numeric_limits<ErrorType>::infinity() };


    [[nodiscard]]
    constexpr auto to_string() const -> std::string
    {
        return fmt::format(
            "{:} at #{:d} with error {:14.6e}",
            converged ? "SUCCESS" : "FAILURE",
            iters, error
        );
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
template<class T, std::invocable<T> G, std::invocable<T, T> DeltaError, std::floating_point ErrorType = long double>
constexpr auto fixed_point_iteration
(
    G g,
    const T& x0,
    DeltaError error,
    const FixedPointIterSettings<ErrorType> settings
) -> FixedPointIterResult<T, ErrorType>
{
    #ifndef NDEBUG
    fmt::println(
        std::cerr,
        "{:s}\n"
        "Starting Point:\n"
        "{}",
        settings.to_string(),
        x0
    );
    #endif

    auto x_curr = x0;
    auto current_error = std::numeric_limits<ErrorType>::infinity();

    for (int i = 0; i < settings.max_iter; ++i)
    {
        const auto x_next = g(x_curr);
        current_error = error(x_next, x_curr);

        #ifndef NDEBUG
        fmt::println(
            std::cerr,
            "#{: >3d}/{: >3d}: {:12.6e}\n"
            "x_next: {}\n"
            "x_curr: {}\n",
            i + 1,
            settings.max_iter,
            current_error,
            x_next,
            x_curr
        );
        #endif

        if (current_error < settings.tolerance)
        {
            return FixedPointIterResult<T, ErrorType>{
                .x = x_next,
                .converged = true,
                .iters = i + 1,
                .error = current_error
            };
        }

        x_curr = x_next;
    }

    return FixedPointIterResult<T, ErrorType>{
        .x = x_curr,
        .converged = false,
        .iters = settings.max_iter,
        .error = current_error
    };
}


template<class T, std::invocable<T> G, std::invocable<T> AbsError, std::floating_point ErrorType = long double>
constexpr auto fixed_point_iteration
(
    G g,
    const T& x0,
    AbsError error,
    const FixedPointIterSettings<ErrorType>& settings
) -> FixedPointIterResult<T, ErrorType>
{
    #ifndef NDEBUG
    fmt::println(
        std::cerr,
        "{:s}\n"
        "\t{:.<40s}: {}",
        settings.to_string(),
        "Starting Point",
        x0
    );
    #endif

    auto x = x0;
    auto current_error = std::numeric_limits<ErrorType>::infinity();
    for (int i = 0; i < settings.max_iter; ++i)
    {
        x = g(x);
        current_error = error(x);

        #ifndef NDEBUG
        fmt::println(
            std::cerr,
            "#{: >5d}/{: >5d}: {:14.6e}",
            i + 1,
            settings.max_iter,
            current_error
        );
        #endif

        if (current_error < settings.tolerance)
        {
            const auto result = FixedPointIterResult<T, ErrorType>{
                .x = x,
                .converged = true,
                .iters = i + 1,
                .error = current_error
            };
            #ifndef NDEBUG
            fmt::println(std::cerr, "{}\n", result.to_string());
            #endif
            return result;
        }
    }

    const FixedPointIterResult<T, ErrorType> result{
        .x = x,
        .converged = false,
        .iters = settings.max_iter,
        .error = current_error
    };
    #ifndef NDEBUG
    fmt::println(std::cerr, "{}\n", result.to_string());
    #endif
    return result;
}


template<class T, std::floating_point DType = long double, std::invocable<T&> GAndAbsError>
constexpr auto fixed_point_iteration
(
    GAndAbsError g,
    T&& x0,
    const FixedPointIterSettings<DType>& settings
) -> FixedPointIterResult<T, DType>
{
    #ifndef NDEBUG
    // fmt::println(
    //     std::cerr,
    //     "{:s}",
    //     // "\t{:.<40s}: {}",
    //     settings.to_string()
    //     // "Starting Point",
    //     // x0
    // );
    #endif

    auto x = std::forward<T>(x0);
    auto current_error = std::numeric_limits<DType>::infinity();

    for (int i = 0; i < settings.max_iter; ++i)
    {
        current_error = g(x);

        #ifndef NDEBUG
        fmt::println(
            std::cerr,
            "#{: >5d}/{: >5d}: {:.12e}",
            i + 1,
            settings.max_iter,
            current_error
        );
        #endif

        if (current_error < settings.tolerance)
        {
            const auto result = FixedPointIterResult<T, DType>{
                .x = std::move(x),
                .converged = true,
                .iters = i + 1,
                .error = current_error
            };
            #ifndef NDEBUG
            fmt::println(std::cerr, "{}\n", result.to_string());
            #endif
            return result;
        }
    }

    const FixedPointIterResult<T, DType> result{
        .x = std::move(x),
        .converged = false,
        .iters = settings.max_iter,
        .error = current_error
    };
    #ifndef NDEBUG
    fmt::println(std::cerr, "{}\n", result.to_string());
    #endif
    return result;
}
#endif // OPTIMIZE_H
