#ifndef LINALG_AXB_UTILS_H
#define LINALG_AXB_UTILS_H

#include <concepts>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include "utils/io.h"

#include "methods/fixed_point.h"

#include "methods/linalg/matrix.h"
#include "methods/linalg/utils/io.h"

#include "methods/linalg/Axb/algorithm.h"
#include "methods/linalg/Axb/linear_system.h"


template<std::floating_point T>
struct IterAxbState : FPState<T>
{
    std::shared_ptr<const LinearSystem<T>> system{};

    std::vector<T> x{};

    [[nodiscard]]
    explicit constexpr IterAxbState(std::shared_ptr<const LinearSystem<T>> Ab)
        : system{ Ab }
        , x(Ab->A.cols(), T{})
    {}

    virtual auto update() -> void
    {
        this->m_iter += 1;
    }

    [[nodiscard]]
    constexpr auto residual() const
    {
        return system->residual(x);
    }

    [[nodiscard]]
    virtual AxbAlgorithm algorithm() const
    {
        std::unreachable();
    }
};


template<std::floating_point T>
struct fmt::formatter<IterAxbState<T>>
{
    enum Style
    {
        Repr,
        Solution,
        Full,
    };

    Style style = Style::Repr;

    fmt::formatter<FPState<T>> underlying{};

    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        auto reached_end = [&](const auto& pos) -> bool
        {
            return pos == ctx.end() or *pos == '}';
        };

        auto it = ctx.begin();

        if (reached_end(it))
            return it;

        this->style = [&] {
            switch (*it++)
            {
                case 'r':
                    return Style::Repr;
                case 's':
                    return Style::Solution;
                case 'F':
                    return Style::Full;
                default:
                    throw std::format_error("Invalid style");
            }
        }();

        if (reached_end(it))
            return it;

        if (*it == ':')
        {
            ++it;
            ctx.advance_to(it);
            it = underlying.parse(ctx);
        }

        return it;
    }

    auto format(const IterAxbState<T>& state, fmt::format_context& ctx) const
    {
        auto out = ctx.out();
        ctx.advance_to(out);
        out = underlying.format(state, ctx);

        if (style == Style::Solution or style == Style::Full)
        {
            out = fmt::format_to(out, ":\n");
            ctx.advance_to(out);
            out = format_vec(state.x, "x", ctx);
        }
        return out;
    }

    auto format_vec(const std::vector<T>& data, const std::string_view label, fmt::format_context& ctx) const
    {
        auto out = ctx.out();
        out = fmt::format_to(out, "{}: [", label);
        bool first = true;
        for (const auto& v : data)
        {
            if (not first)
                out = fmt::format_to(out, " ");

            ctx.advance_to(out);
            underlying.real_fmt.format(v, ctx);
            first = false;
        }
        return fmt::format_to(out, "]");
    }
};


#endif //LINALG_AXB_UTILS_H
