#ifndef FIXED_POINT_ALGORITHM_H
#define FIXED_POINT_ALGORITHM_H

#include <memory>
#include <utility>
#include <limits>

#include <fmt/format.h>

#ifndef NDEBUG
#include <iostream>
#include <fmt/ostream.h>
#endif

#include "settings.h"

template<std::floating_point ErrorType>
class FixedPoint {
    protected:
        FPSettings<ErrorType> iter_settings{};

    public:
        [[nodiscard]]
        explicit constexpr FixedPoint(const FPSettings<ErrorType>& fps) : iter_settings{fps} {}

        template<class State, class... Args>
        [[nodiscard]]
        auto solve(Args&&... args) const
        {
            auto state = std::make_unique<State>(std::forward<Args>(args)...);

            while (
                state->iteration() < iter_settings.max_iter and
                not state->converged(iter_settings.tolerance)
            )
            {
                #ifndef NDEBUG
                fmt::println(std::cerr, "{}", *state);
                #endif
                state->update();
            }

            return std::make_pair(state->converged(iter_settings.tolerance), std::move(state));
        }
};




#endif //FIXED_POINT_H
