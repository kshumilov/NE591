#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <concepts>
#include <functional>


template<std::floating_point scalar_t>
constexpr auto fixed_point_iteration(
    std::function<scalar_t(scalar_t)> step,
    const scalar_t guess,
    std::function<bool(scalar_t, scalar_t)> next
) -> scalar_t
{
    scalar_t prev = guess;
    scalar_t curr = step(guess);

    while (next(prev, curr)) {
        prev = curr;
        curr = step(curr);
    }

    return curr;
}
#endif //OPTIMIZE_H
