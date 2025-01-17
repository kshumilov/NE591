#ifndef FX_H
#define FX_H

#include <cmath>

inline auto user_func(const double x) -> double {
    return std::sin(x) * std::exp(-x * x);
}

#endif //FX_H
