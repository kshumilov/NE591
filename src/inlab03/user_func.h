#ifndef FX_H
#define FX_H

#include <cmath>

/**
 * \brief User defined function
 *
 * As an example, this function defines sine modulated gaussian.
 * Traditional std::log, std::exp, std::cos, etc, are available in the header <cmath>
 *
 * see https://cplusplus.com/reference/cmath/
 *
 * @param x real value
 */
inline auto user_func(const double x) -> double {
    return std::exp(x);
}

#endif //FX_H
