#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <functional>


template<class T>
constexpr auto fixed_point_iteration(
    std::function<T(T)> step,
    const T guess,
    std::function<bool(T, T)> next
) -> T
{
    T prev = guess;
    T curr = step(guess);

    while (next(prev, curr)) {
        prev = curr;
        curr = step(curr);
    }

    return curr;
}
#endif //OPTIMIZE_H
