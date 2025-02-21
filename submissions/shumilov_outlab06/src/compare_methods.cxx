#include "methods/linalg/matrix.h"

#include <iostream>
#include <vector>
#include <ranges>
#include <utility>
#include <chrono>
#include <methods/array.h>

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/color.h>

#include "methods/linalg/lu.h"
#include "methods/linalg/Axb/solve.h"

#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>

#include "build_system.h"

using json = nlohmann::json;

using real = long double;
using LinearSystem = std::pair<Matrix<real>, std::vector<real>>;


constexpr auto power_of_two(int p) -> std::size_t {
    std::size_t result{ 1 };
    for (int i{}; i < p; ++i) {
        result *= 2;
    }
    return result;
}


struct TimingInfo {
    int n{};
    std::string algo{};
    long long time{};  // miliseconds
    real error{};
    int iterations{};
    bool converged{};
    real max_deviation{};

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_ONLY_SERIALIZE(TimingInfo, n, algo, time, error, iterations, converged, max_deviation)
};


int main(int argc, char* argv[]) {
    argparse::ArgumentParser program{
        "compare_methods",
        "1.0",
        argparse::default_arguments::help,
    };

    program.add_description("Compares performance of LUP, GS, PJ, and SOR methods for solving Ax=b problem");

    program.add_argument("-s")
               .help("Smallest power of two to generate rank: n = 2^s")
               .scan<'i', int>()
               .default_value(5);

    program.add_argument("-l")
               .help("Largest power of two to generate rank: n = 2^s")
               .scan<'i', int>()
               .default_value(8);

    program.add_argument("--iter", "--iterations")
               .help("Maximum number of iterations")
               .scan<'i', int>()
               .default_value(100);

    program.add_argument("--tol", "--tolerance")
               .help("Maximum tolerance")
               .scan<'g', real>()
               .default_value(real{1e-4});

    program.add_argument("-w", "--relaxation-factor")
               .help("Relaxation factor for SOR, ignored otherwise")
               .scan<'g', real>()
               .default_value(real{1.8});

    try {
        program.parse_args(argc, argv);
        const auto smallest = program.get<int>("-s");
        const auto largest = program.get<int>("-l");

        const auto powers = std::views::iota(smallest, largest + 1);
        std::vector<LinearSystem> systems{};
        for (const auto p : powers) {
            systems.emplace_back(build_system<real>(power_of_two(p)));
        }

        const FixedPointIterSettings settings{
            program.get<real>("--tolerance"),
            program.get<int>("--iterations")
        };
        const real relaxation_factor = program.get<real>("--relaxation-factor");

        std::vector<TimingInfo> timing_infos{};
        for (auto& system : systems) {
            auto& A = std::get<0>(system);
            const auto& b = std::get<1>(system);

            fmt::println("Testing matrix {}", A.shape_info());

            auto start = std::chrono::high_resolution_clock::now();
            const auto pj = point_jacobi<real>(A, b, settings);
            auto end = std::chrono::high_resolution_clock::now();
            const auto pj_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            TimingInfo pj_timing{
                static_cast<int>(A.rows())
              , "PJ"
              , pj_time.count()
              , pj.error
              , pj.iters
              , pj.converged
            };

            start = std::chrono::high_resolution_clock::now();
            const auto gs = gauss_seidel<real>(A, b, settings);
            end = std::chrono::high_resolution_clock::now();
            const auto gs_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            TimingInfo gs_timing{ static_cast<int>(A.rows()), "GS", gs_time.count(), gs.error, gs.iters, gs.converged };

            start = std::chrono::high_resolution_clock::now();
            const auto sor = successive_over_relaxation<real>(A, b, relaxation_factor, settings);
            end = std::chrono::high_resolution_clock::now();
            const auto sor_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            TimingInfo sor_timing{
                static_cast<int>(A.rows())
              , "SOR"
              , sor_time.count()
              , sor.error
              , sor.iters
              , sor.converged
            };

            start = std::chrono::high_resolution_clock::now();
            const auto [P, success] = lup_factor_inplace<real>(A);
            const auto x = lup_solve<real>(A, P, b);
            end = std::chrono::high_resolution_clock::now();

            std::vector<real> tmp(x.size());
            std::vector<real> residual{ b.cbegin(), b.cend() };
            gemv<real, MatrixSymmetry::Upper>(A, x, tmp, real{ 1 });
            gemv<real, MatrixSymmetry::Lower, Diag::Unit>(A, tmp, residual, real{ -1 }, real{ 1 });
            const auto error = max_abs(residual);
            const auto lup_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            fmt::println("LUP Residual {:12.6e} in {:%S} sec", error, lup_time);

            TimingInfo lup_timing{ static_cast<int>(A.rows()), "LUP", lup_time.count(), error, 0, true, 0 };

            pj_timing.max_deviation = max_rel_error(pj.x, x);
            gs_timing.max_deviation = max_rel_error(gs.x, x);
            sor_timing.max_deviation = max_rel_error(sor.x, x);

            fmt::println(
                "PJ  {} in {:%S} sec (LUP Deviation = {:12.6})"
              , pj.to_string()
              , pj_time
              , pj_timing.max_deviation
            );

            fmt::println(
                "GS  {} in {:%S} sec (LUP Deviation = {:12.6})"
              , gs.to_string()
              , gs_time
              , gs_timing.max_deviation
            );

            fmt::println(
                "SOR {} in {:%S} sec (LUP Deviation = {:12.6})"
              , sor.to_string()
              , sor_time
              , sor_timing.max_deviation
            );

            timing_infos.emplace_back(pj_timing);
            timing_infos.emplace_back(gs_timing);
            timing_infos.emplace_back(sor_timing);
            timing_infos.emplace_back(lup_timing);

            std::cout << std::endl;
        }

        json j = timing_infos;
        std::ofstream o("timing_info.json");
        o << std::setw(4) << j << std::endl;
        o.close();
    }
    catch (const std::exception& err) {
        std::cerr << "\n"
          << format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
          << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
