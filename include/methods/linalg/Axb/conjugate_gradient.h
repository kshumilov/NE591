#ifndef CONJUGATE_GRADIENT_H
#define CONJUGATE_GRADIENT_H

#include <concepts>
#include <memory>
#include <vector>

#include <fmt/core.h>

#include "methods/fixed_point.h"
#include "methods/linalg/matrix.h"
#include "methods/linalg/Axb/utils.h"
#include "methods/linalg/Axb/algorithm.h"
#include "methods/linalg/utils/math.h"


struct CGParams
{
    int residual_update_frequency{10};

    [[nodiscard]]
    constexpr auto update_residual(const int iter) const
    {
        return iter % residual_update_frequency == 0;
    }
};


template<>
struct fmt::formatter<CGParams>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    [[nodiscard]]
    constexpr auto format(const CGParams& params, format_context& ctx) const
    {
        return fmt::format_to(
            ctx.out(),
            "Accurate Residual Update Frequency: {:L}",
            params.residual_update_frequency
        );
    }
};


template<std::floating_point T>
struct CGState final : IterAxbState<T>
{
    const CGParams params{};

    std::vector<T> r{};
    std::vector<T> d{};

    [[nodiscard]]
    constexpr CGState(
        std::shared_ptr<const LinearSystem<T>> Ab,
        const CGParams params_
    ) : IterAxbState<T>{Ab}
      , params{ params_ }
      , r(Ab->b.cbegin(), Ab->b.cend())
      , d(Ab->b.cbegin(), Ab->b.cend())
    {
        CGState::validate_system(*this->system);
        this->m_error = norm_l2(r) / norm_l2(this->system->b);
    }

    static auto validate_system(const LinearSystem<T>& system)
    {
        const auto& A = system.A;


        if (const auto idx = find_matrix_assymetry<T>(A, T{}, 1e-12);
            idx.has_value())
        {
            const auto& [i, j] = idx.value();
            throw std::invalid_argument(
                fmt::format("`A` is asymmetric in ({}, {}): {} != {}", i, j, A[i, j], A[j, i])
            );
        }
    }

    void update() override
    {
        const auto& A = this->system->A;
        const auto& b = this->system->b;
        auto& x = this->x;

        const auto Ad = A * d;

        const auto rprev_dot_rprev = dot(r, r);
        const auto alpha = rprev_dot_rprev / dot(d, Ad);

        // Update the solution
        axpy<T>(d, x, alpha);

        // Get new residual
        if (params.update_residual(this->iteration()))
        {
            std::copy(b.cbegin(), b.cend(), r.begin());
            gemv<T>(A, x, r, -T{ 1 }, T{ 1 });
        }
        else
        {
            axpy<T>(Ad, r, -alpha);
        }

        // Get new Conjugate direction
        const auto r_dot_r = dot(r, r);
        const auto beta = r_dot_r / rprev_dot_rprev;
        scal<T>(d, beta);
        axpy<T>(r, d);

        this->m_error = std::sqrt(r_dot_r) / norm_l2(b);

        FPState<T>::update();
    }

    [[nodiscard]]
    AxbAlgorithm algorithm() const override
    {
        return AxbAlgorithm::ConjugateGradient;
    }
};


template<std::floating_point T>
struct fmt::formatter<CGState<T>>
{
    formatter<IterAxbState<T>> underlying{};

    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return underlying.parse(ctx);
    }

    auto format(const CGState<T>& state, format_context& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "CG :");
        ctx.advance_to(out);
        out = underlying.format(state, ctx);

        if (underlying.style == decltype(underlying)::Style::Full)
        {
            out = fmt::format_to(out, "\n");
            ctx.advance_to(out);
            out = underlying.format_vec(state.r, "r", ctx);

            out = fmt::format_to(out, "\n");
            ctx.advance_to(out);
            out = underlying.format_vec(state.d, "d", ctx);
        }
        return out;
    }
};


template<std::floating_point T>
struct CG : FixedPoint<T>
{
    CGParams params{};

    [[nodiscard]]
    explicit constexpr CG(
        const FPSettings<T>& fps,
        const CGParams params_ = CGParams{}
    ) : FixedPoint<T>{ fps }
      , params{ params_ }
    {}


    [[nodiscard]]
    auto solve(std::shared_ptr<const LinearSystem<T>> system) const
    {
        return FixedPoint<T>::template solve<CGState<T>>(system, params);
    }
};

template<std::floating_point T>
struct fmt::formatter<CG<T>>
{
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    [[nodiscard]]
    constexpr auto format(const CG<T>& cg, format_context& ctx) const
    {
        return fmt::format_to(
            ctx.out(),
            "Method: {}\n"
            "{}",
            cg.algorithm(),
            cg.params
        );
    }
};

#endif //CONJUGATE_GRADIENT_H
