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
class FPState
{
    protected:
        ErrorType m_error{ std::numeric_limits<ErrorType>::infinity() };
        int m_iter{};

    public:
        constexpr FPState() = default;

        [[nodiscard]] constexpr auto error() const noexcept { return m_error; }
        [[nodiscard]] constexpr auto iteration() const noexcept { return m_iter; }
        [[nodiscard]] constexpr auto converged(const ErrorType tolerance) { return error() < tolerance; }

        virtual ~FPState() = default;
        virtual auto update() -> void
        {
            m_iter += 1;
        }
};


template<std::floating_point ErrorType>
struct fmt::formatter<FPState<ErrorType>>
{
    fmt::formatter<int> int_fmt{};
    fmt::formatter<ErrorType> real_fmt{};

    fmt::parse_context<> default_int_fmt{ ">5L" };
    fmt::parse_context<> default_real_fmt{ "14.8g" };


    constexpr auto parse(fmt::format_parse_context& ctx)
    {
        auto reached_end = [&](const auto& pos) -> bool
        {
            return pos == ctx.end() or *pos == '}';
        };

        auto it = ctx.begin();

        // Check that everything is defaulted
        if (reached_end(it))
        {
            int_fmt.parse(default_int_fmt);
            real_fmt.parse(default_real_fmt);
            return it;
        }

        // Parse String format
        if (*it != ':')
            it = int_fmt.parse(ctx);
        else
            int_fmt.parse(default_int_fmt);

        if (reached_end(it))
        {
            real_fmt.parse(default_real_fmt);
            return it;
        }

        // Find the ':' divider and move past it
        if (*it == ':')
            ++it;

        // Parse string specification
        ctx.advance_to(it);
        return real_fmt.parse(ctx);
    }


    auto format(const FPState<ErrorType>& state, fmt::format_context& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "Iter #");
        ctx.advance_to(out);
        out = int_fmt.format(state.iteration(), ctx);
        ctx.advance_to(out);

        out = fmt::format_to(ctx.out(), ", Error = ");
        ctx.advance_to(out);
        return real_fmt.format(state.error(), ctx);
    }
};


template<std::floating_point ErrorType>
class FixedPoint {
    protected:
        FixedPointSettings<ErrorType> iter_settings{};

    public:
        [[nodiscard]]
        explicit constexpr FixedPoint(const FixedPointSettings<ErrorType>& fps) : iter_settings{fps} {}

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
                fmt::println(std::cerr, "{:r::14.6e}", *state);
                #endif
                state->update();
            }

            return std::make_pair(state->converged(iter_settings.tolerance), std::move(state));
        }
};




#endif //FIXED_POINT_H
