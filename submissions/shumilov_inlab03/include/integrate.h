#ifndef INTEGRATE_H
#define INTEGRATE_H

#include <concepts>
#include <stdexcept>
#include <ranges>


enum class Quadrature {
    Trapezoidal,
    Simpson,
    Gauss,
};


template <std::floating_point scalar_t>
auto trapezoidal(std::span<const scalar_t> y, const scalar_t step = scalar_t{0.0}) -> scalar_t
{
    return step * (
        (y.back() + y.front()) / 2.0 +
        std::reduce(
            y.cbegin() + 1, y.cend() - 1,
            scalar_t{0.}, std::plus<scalar_t>{}
        )
    );
}


template <std::floating_point scalar_t>
auto simpson(std::span<const scalar_t> y, const scalar_t step = scalar_t{0.0}) -> scalar_t
{
    scalar_t result {y.front() + y.back()};
    for (const auto i : std::views::iota(1U, y.size() - 1U)) {
        result += y[i] * (i % 2 ? 4 : 2);
    }
    return step / 3.0 * result;
}


template <std::floating_point scalar_t>
auto gauss(std::span<const scalar_t> y, const scalar_t step = scalar_t{0.0}) -> scalar_t
{
    throw std::runtime_error("gauss not implemented");
}


template <std::floating_point scalar_t>
auto integrate(std::span<const scalar_t> y, Quadrature quad, const scalar_t step = scalar_t{0.0}) -> scalar_t
{
    switch (quad) {
        case Quadrature::Trapezoidal: {
            return trapezoidal(y, step);
        }
        case Quadrature::Simpson: {
            return simpson(y, step);
        }
        case Quadrature::Gauss: {
            return gauss(y, step);
        }
        default: {
            throw std::runtime_error("Invalid quadrature type");
        }
    }
}

#endif //INTEGRATE_H
