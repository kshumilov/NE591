#include "methods/linalg/matrix.h"

#include <iostream>
#include <vector>
#include <array>
#include <ranges>
#include <utility>
#include <chrono>
#include <methods/array.h>

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/color.h>

#include "methods/linalg/lu.h"
#include "methods/linalg/Axb/solve.h"
#include "methods/array.h"

#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>

#include "custom_system.h"

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
    std::chrono::duration<long long, std::nano> time{};  // nanoseconds
    real relative_error{};
    real residual_error{};
    int iterations{};
    bool converged{};

    TimingInfo() = default;

    TimingInfo(
        const int n_, const std::string& algo_,
        const std::chrono::duration<long long, std::nano> time_,
        const real relative_error_, const real residual_error_,
        const int iterations_, const bool converged_
    )
    : n{n_} , algo{algo_}, time{time_}, relative_error{relative_error_}
    , residual_error{ residual_error_ }, iterations{ iterations_ }, converged{ converged_ }
    {}

    TimingInfo(
        const int n_, const std::string& algo_,
        const IterativeAxbResult<real>& result,
        const std::chrono::duration<long long, std::nano> time_
    )
            : n{n_}
            , algo{algo_}
            , time{ time_}
            , relative_error{ result.relative_error }
            , residual_error{ result.residual_error }
            , iterations{ result.iters }
            , converged{ result.converged }
    {
    }

    [[nodiscard]] auto to_string() const -> std::string {
        return fmt::format("{:3s} {:10s} in #{:5d} with relative error = {:12.6e}, residual error = {:12.6e} in {:%S} sec",
            algo, converged ? "converged" : "failed", iterations, relative_error, residual_error, time
        );
    }

    template<class BasicJsonType>
    friend void to_json(BasicJsonType& j, const TimingInfo& ti) {
        j["n"] = ti.n;
        j["algo"] = ti.algo;
        j["time"] = ti.time.count();
        j["relative_error"] = ti.relative_error;
        j["residual_error"] = ti.residual_error;
        j["iterations"] = ti.iterations;
        j["converged"] = ti.converged;
    }
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

    program.add_argument("--output-json")
           .help("Path to json-formatted timings");

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
              , pj
              , pj_time
            };

            start = std::chrono::high_resolution_clock::now();
            const auto gs = gauss_seidel<real>(A, b, settings);
            end = std::chrono::high_resolution_clock::now();
            const auto gs_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            TimingInfo gs_timing{
                static_cast<int>(A.rows())
              , "GS"
              , gs
              , gs_time
            };

            start = std::chrono::high_resolution_clock::now();
            const auto sor = successive_over_relaxation<real>(A, b, relaxation_factor, settings);
            end = std::chrono::high_resolution_clock::now();
            const auto sor_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            TimingInfo sor_timing{
                static_cast<int>(A.rows())
              , "SOR"
              , sor
              , sor_time
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

            TimingInfo lup_timing{static_cast<int>(A.rows()), "LUP", lup_time, 0, error, 0, true};

            fmt::println("{}", pj_timing.to_string());
            fmt::println("{}", gs_timing.to_string());
            fmt::println("{}", sor_timing.to_string());

            timing_infos.emplace_back(pj_timing);
            timing_infos.emplace_back(gs_timing);
            timing_infos.emplace_back(sor_timing);
            timing_infos.emplace_back(lup_timing);

            std::cout << std::endl;
        }

        if (const auto output_filename = program.present<std::string>("--output-json");
            output_filename.has_value()
        ) {
            std::ofstream output{ output_filename.value() };
            if (!output.is_open()) {
                throw std::runtime_error(
                    fmt::format("Could not open: '{}'", output_filename.value())
                ); // Indicate an error occurred
            }
            json j = timing_infos;
            output << std::setw(4) << j << std::endl;
            output.close();
        }
    }
    catch (const std::exception& err) {
        std::cerr << "\n"
          << format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
          << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
