#ifndef PRECOND_CONJUGATE_GRADIENT_H
#define PRECOND_CONJUGATE_GRADIENT_H

#include <concepts>
#include <memory>
#include <vector>

#include <fmt/format.h>

#include "methods/linalg/matrix.h"
#include "methods/linalg/utils/math.h"

#include "methods/fixed_point.h"

#include "methods/linalg/Axb/algorithm.h"
#include "methods/linalg/Axb/state.h"
#include "methods/linalg/Axb/linear_system.h"

enum class PreconditionerType
{
    Jacobi
};

template<std::floating_point T>
struct Preconditioner
{
    virtual ~Preconditioner() = default;

    virtual void apply(const std::vector<T>& x, std::vector<T>& y) const = 0;

    [[nodiscard]]
    constexpr auto apply(const std::vector<T>& x) const
    {
        std::vector<T> result(x.size());
        this->apply(x, result);
        return result;
    }

    [[nodiscard]]
    virtual Matrix<T> get_full_matrix() const = 0;
};


template<std::floating_point T>
struct JacobiPreconditioner final : Preconditioner<T> {
    std::vector<T> inv_diag{};

    [[nodiscard]]
    constexpr explicit JacobiPreconditioner(const LinearSystem<T>& system)
        : inv_diag(system.A.diagonal())
    {
        for (auto& v : inv_diag)
            v = 1 / v;
    }

    [[nodiscard]]
    constexpr auto get_full_matrix() const -> Matrix<T> override
    {
        return Matrix<T>::diagonal(inv_diag);
    }

    constexpr void apply(const std::vector<T>& x, std::vector<T>& y) const override
    {
        std::transform(
            x.cbegin(), x.cend(),
            inv_diag.cbegin(),
            y.begin(),
            std::multiplies<T>()
        );
    }
};


template<std::floating_point T>
auto make_preconditioner(const PreconditionerType type, const LinearSystem<T>& system)
{
    switch (type)
    {
        case PreconditionerType::Jacobi:
            return std::unique_ptr<Preconditioner<T>>(new JacobiPreconditioner<T>(system));
        default:
            std::unreachable();
    }
}

struct PCGParams
{
    int residual_update_frequency{10};
    PreconditionerType preconditioner_type{PreconditionerType::Jacobi};

    [[nodiscard]]
    constexpr auto update_residual(const int iter) const
    {
        return iter % residual_update_frequency == 0;
    }
};


// template<>
// struct fmt::formatter<PCGParams>
// {
//     [[nodiscard]]
//     constexpr auto parse(format_parse_context& ctx)
//     {
//         return ctx.begin();
//     }
//
//     [[nodiscard]]
//     constexpr auto format(const CGParams& params, format_context& ctx) const
//     {
//         return fmt::format_to(
//             ctx.out(),
//             "Accurate Residual Update Frequency: {:L}",
//             params.residual_update_frequency
//         );
//     }
// };


template<std::floating_point T>
struct PCGState final : IterAxbState<T>
{
    const PCGParams params{};

    std::vector<T> d{};
    std::unique_ptr<const Preconditioner<T>> M{};

    [[nodiscard]]
    constexpr PCGState(
        std::shared_ptr<const LinearSystem<T>> Ab,
        const PCGParams params_
    ) : IterAxbState<T>{Ab}
      , params{ params_ }
      , d(Ab->rank(), T{})
      , M{ std::move(make_preconditioner(params.preconditioner_type, *this->system)) }
    {
        PCGState::validate_system(*this->system);

        this->M->apply(this->r, d);
        this->m_error = norm_l2(this->r) / norm_l2(this->system->b);
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

        auto z = M->apply(this->r);
        const auto Ad = A * d;

        const auto r_dot_z_prev = dot(this->r, z);
        const auto alpha = r_dot_z_prev / dot(d, Ad);

        // Update the solution
        axpy<T>(d, this->x, alpha);

        // Get new residual
        if (params.update_residual(this->iteration()))
            this->update_residual();
        else
            axpy<T>(Ad, this->r, -alpha);

        M->apply(this->r, z);

        // Get new Conjugate direction
        const auto r_dot_z = dot(this->r, z);
        const auto beta = r_dot_z / r_dot_z_prev;
        scal<T>(d, beta);
        axpy<T>(z, d);

        this->m_error = norm_l2(this->r) / norm_l2(b);

        FPState<T>::update();
    }

    [[nodiscard]]
    AxbAlgorithm algorithm() const override
    {
        return AxbAlgorithm::PreCondConjugateGradient;
    }
};


template<std::floating_point T>
struct fmt::formatter<PCGState<T>>
{
    formatter<IterAxbState<T>> underlying{};

    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx)
    {
        return underlying.parse(ctx);
    }

    auto format(const PCGState<T>& state, format_context& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "PCG:");
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
struct PCG : FixedPoint<T>
{
    PCGParams params{};

    [[nodiscard]]
    explicit constexpr PCG(
        const FPSettings<T>& fps,
        const PCGParams params_ = PCGParams{}
    ) : FixedPoint<T>{ fps }
      , params{ params_ }
    {}


    [[nodiscard]]
    auto solve(std::shared_ptr<const LinearSystem<T>> system) const
    {
        return FixedPoint<T>::template solve<PCGState<T>>(system, params);
    }
};
//
// template<std::floating_point T>
// struct fmt::formatter<CG<T>>
// {
//     [[nodiscard]]
//     constexpr auto parse(format_parse_context& ctx)
//     {
//         return ctx.begin();
//     }
//
//     [[nodiscard]]
//     constexpr auto format(const CG<T>& cg, format_context& ctx) const
//     {
//         return fmt::format_to(
//             ctx.out(),
//             "Method: {}\n"
//             "{}",
//             cg.algorithm(),
//             cg.params
//         );
//     }
// };
//
#endif //PRECOND_CONJUGATE_GRADIENT_H
