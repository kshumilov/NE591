#ifndef DIFFUSION_SOLVER_H
#define DIFFUSION_SOLVER_H

// 3rd-party Dependencies
#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <nlohmann/json.hpp>

#include "methods/linalg/lu.h"
#include "methods/linalg/matrix.h"
#include "methods/linalg/Axb/utils.h"
#include "methods/optimize.h"

#include "project/diffusion_problem.h"

using json = nlohmann::json;


struct LUPSolver
{
  template<class T>
  struct Solution
  {
    const IsotropicSteadyStateDiffusion2D<T>& problem;
    const Matrix<T> scalar_flux;
    const std::vector<T> residual;


    [[nodiscard]]
    constexpr auto to_string() const -> std::string
    {
      std::string result{};
      // result.append("================================================================================\n");
      result.append(problem.to_string());
      result.append(
        "\n================================================================================\n"
      );
      fmt::format_to(std::back_inserter(result), "{:^80s}\n", "Results");
      result.append(
        "--------------------------------------------------------------------------------\n"
      );
      fmt::format_to(
        std::back_inserter(result),
        "Flux, phi{}:\n",
        scalar_flux.shape_info()
      );
      result.append(scalar_flux.to_string());
      fmt::format_to(
        std::back_inserter(result),
        "\nMax Abs Residual: {:12.6e}",
        max_abs(residual)
      );
      result.push_back('\n');
      result.append(
        "================================================================================\n"
      );
      return result;
    }


    auto echo(std::ostream& out) const -> void
    {
      out << to_string() << std::endl;
    }


    template<class BasicJsonType>
    friend void to_json(BasicJsonType& j, const Solution& solution)
    {
      j["flux"] = solution.scalar_flux;
      j["problem"] = solution.problem;
      j["residual"] = solution.residual;
    }
  };


  template<class T>
  [[nodiscard]] Solution<T> solve(const IsotropicSteadyStateDiffusion2D<T>& problem) const
  {
    problem.validate();

    auto A = build_operator(problem);
    auto b = build_rhs(problem);

    const auto& [P, lu_result] = lup_factor_inplace<T>(A);

    if (lu_result == LUResult::SmallPivotEncountered)
    {
      std::cerr << fmt::format(
          fmt::emphasis::bold | fg(fmt::color::red),
          "Error: Small Pivot Encountered"
        )
        << std::endl;
    }

    std::vector<T> x = lup_solve<T>(A, P, b);
    std::vector<T> residual = calculate_residual<T>(A, x, b);

    return Solution<T>{
      .problem = problem,
      .scalar_flux = Matrix<T>(
        static_cast<std::size_t>(problem.grid.points.NX),
        static_cast<std::size_t>(problem.grid.points.NY),
        std::move(x)
      ),
      .residual = std::move(residual)
    };
  }


  template<class T>
  [[nodiscard]] constexpr auto build_operator(const IsotropicSteadyStateDiffusion2D<T>& problem) const -> Matrix<T>
  {
    const auto dim = problem.grid.points.size();

    return Matrix<T>::from_func(
      static_cast<std::size_t>(dim),
      [&](const auto i, const auto j) -> T
      {
        return problem.operator_element(i, j);
      }
    );
  }


  template<class T>
  [[nodiscard]] constexpr auto build_rhs(const IsotropicSteadyStateDiffusion2D<T>& problem) const -> std::span<const T>
  {
    return problem.source.data();
  }


  template<class T>
  [[nodiscard]] constexpr auto calculate_residual
  (
    const Matrix<T>& LU,
    std::span<const T> x,
    std::span<const T> b
  ) const -> std::vector<T>
  {
    std::vector<T> Ux(LU.cols());
    gemv<T, MatrixSymmetry::Upper>(LU, x, Ux);

    std::vector<T> res{ b.cbegin(), b.cend() };
    gemv<T, MatrixSymmetry::Lower, Diag::Unit>(LU, Ux, res, T{ -1.0 });

    return res;
  }
};


template<std::floating_point DType>
constexpr auto successive_over_relaxation_sparse(
        const IsotropicSteadyStateDiffusion2D<DType>& problem,
        std::span<const DType> b,
        const DType relaxation_factor,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
    assert(relaxation_factor >= 1.0);

    std::vector<DType> x(b.size());
    std::vector<DType> x_next(b.size());

    auto g = [&](std::span<DType> x_curr) constexpr -> std::span<DType>
    {
        for (std::size_t i{}; i < x_curr.size(); ++i)
        {
            DType diag{};
            DType dot_prod{};

            for (const std::vector<std::pair<std::size_t, DType>> nonzero = problem.nonzero_row_elems(i);
                 const auto& [j, value] : nonzero)
            {
                if (j < i)
                    dot_prod += value * x_next[j];
                else if (j == i)
                    diag = value;
                else
                    dot_prod += value * x_curr[j];
            }

            x_next[i] = (1 - relaxation_factor) * x_curr[i]
                        + relaxation_factor * (b[i] - dot_prod) / diag;
        }

        std::swap(x, x_next);

        return std::span{x};
    };

    const auto iter_result = fixed_point_iteration<std::span<DType>>(
       g, x, max_rel_diff<std::span<const DType>, std::span<const DType>>, settings
    );

    std::vector<DType> residual{b.cbegin(), b.cend()};
    problem.matvec(x, residual, DType{1}, DType{-1});

    return IterativeAxbResult<DType>{
        .x = std::move(x),
        .relative_error = iter_result.error,
        .residual_error = max_abs(residual),
        .converged = iter_result.converged,
        .iters = iter_result.iters
    };
}


template<std::floating_point DType>
constexpr auto gauss_seidel_sparse(
        const IsotropicSteadyStateDiffusion2D<DType>& problem,
        std::span<const DType> b,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
  return successive_over_relaxation_sparse<DType>(problem, b, 1.0, settings);
}


template<std::floating_point DType>
constexpr auto point_jacobi_sparse(
        const IsotropicSteadyStateDiffusion2D<DType>& problem,
        std::span<const DType> b,
        const FixedPointIterSettings<DType> settings = FixedPointIterSettings{}
) -> IterativeAxbResult<DType>
{
    std::vector<DType> x(b.size());
    std::vector<DType> x_next(b.size());

    auto g = [&](std::span<DType> x_curr) constexpr -> std::span<DType>
    {
        for (std::size_t i{}; i < x_curr.size(); ++i)
        {
            DType diag{};
            DType dot_prod{};

            for (const std::vector<std::pair<std::size_t, DType>> nonzero = problem.nonzero_row_elems(i);
                 const auto[j, value] : nonzero)
            {
                if (j < i)
                    dot_prod += value * x_curr[j];
                else if (j == i)
                    diag = value;
                else
                    dot_prod += value * x_curr[j];
            }

            x_next[i] = (b[i] - dot_prod) / diag;
        }

        std::swap(x, x_next);

        return std::span{x};
    };

    const auto iter_result = fixed_point_iteration<std::span<DType>>(
       g, x, max_rel_diff<std::span<const DType>, std::span<const DType>>, settings
    );

    std::vector<DType> residual{b.cbegin(), b.cend()};
    problem.matvec(x, residual, DType{1}, DType{-1});

    return IterativeAxbResult<DType>{
        .x = std::move(x),
        .relative_error = iter_result.error,
        .residual_error = max_abs(residual),
        .converged = iter_result.converged,
        .iters = iter_result.iters
    };
}
#endif // DIFFUSION_SOLVER_H
