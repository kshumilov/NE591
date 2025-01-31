#include <cmath>

#include "user_func.h"

/**
 * \brief User defined function
 *
 * As an example, this function defines sine modulated gaussian.
 * Traditionl std::log, std::exp, std::cos, etc, are availbe in the header <cmath>
 *
 * see https://cplusplus.com/reference/cmath/
 *
 * @param x real value
 */
auto user_func(const double x) -> double {
    return std::exp(x);
}
