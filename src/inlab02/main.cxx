#include <ranges>
#include <format>
#include <argparse/argparse.hpp>

#include "interpolate.h"
#include "array.h"

#include "user_func.h"


using dvec = std::vector<double>;

auto outlab02(int samples, const dvec& xi, const dvec& yi, bool use_fx = false) -> std::pair<dvec, dvec> {
    auto xs = linspace(xi[0], xi[xi.size() - 1], samples);
    const LagrangeInterpolation<double> inter {xi, yi};
    auto Ls = inter(xs);

    // dvec Es(xs.size(), 0.0);
    // for (int i = 0; i < xs.size(); i++) {
    //     Es[i] = user_func(xs[i]) - Ls[i];
    // }

    return {xs, Ls};
}


auto echo_output(const int m, const dvec& xs, const dvec& ys, const bool use_fx) -> void {
    std::cout << std::format("{:=^80s}\n", "")
                          << std::format("{: ^80s}\n", "Input Arguments")
                          << std::format("{:-^80s}\n", "")
                          << std::format("#samples: m = {:d}\n", m)
                          << std::format("#points : n = {:d}\n", xs.size())
                          << std::format("user defined function: {}\n", use_fx)
                          << std::format("{:-^80s}\n", "")
                          << std::format("{: ^26s}{: ^27s}{: ^27s}\n", "ID", "x", "f(x)");

    for (const auto [i, x, y] : std::views::zip(std::views::iota(1), xs, ys)) {
        std::print(std::cout, "{: >26d}", i);
        std::print(std::cout, "{: >26g}", x);
        std::println(std::cout, "{: >26g}", y);
    }

    std::println(std::cout, "{:=^80s}", "");
}

auto main(int argc, char* argv[]) -> int {
    using namespace std::literals;

    argparse::ArgumentParser program {
        "shumilov_inlab02",
        "1.0",
        argparse::default_arguments::help,
    };

    const std::string description {
        "================================================================================\n"s +
        std::format("{:<80s}\n", "NE 591 Outlab #01: Basic Math") +
        std::format("{:<80s}\n", "Author: Kirill Shumilov") +
        std::format("{:<80s}\n", "01/17/2025") +
        "================================================================================\n"s
        "This program perform Lagrange Interpolation of a 1D real function\n"s
      };

    program.add_description(description);

    program.add_argument("-n")
        .help("Number of interpolation points")
        .scan<'d', int>();

    program.add_argument("-m")
        .help("Number of points where the Lagrange Interpolation Polynomial will be evaluated.")
        .required()
        .scan<'d', int>();

    program.add_argument("-x")
        .help("Distinct interpolation points, x, sorted from smallest to greatest")
        .append()
        .nargs(argparse::nargs_pattern::at_least_one)
        .required()
        .scan<'g', double>();

    program.add_argument("-y")
        .help("Function values at interpolation points, len(y) == len(x)")
        .append()
        .nargs(argparse::nargs_pattern::at_least_one)
        .scan<'g', double>();

    program.add_argument("--use-fx")
        .help("Toggle the use of user-defined function")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
        const auto xi = program.get<dvec>("x");

        if (xi.size() < 2U) {
            throw std::invalid_argument("At least two interpolations points must be provided");
        }

        if (not std::ranges::is_sorted(xi)) {
            throw std::invalid_argument("x is not sorted");
        }

        auto m = program.get<int>("m");
        if (m < 1) {
            throw std::invalid_argument("Number of interpolated points must be >= 1");
        }

        auto use_fx = program.get<bool>("--use-fx");
        if (not use_fx) {
            const auto yi = program.get<dvec>("y");
            if (xi.size() != yi.size()) {
                throw std::invalid_argument("len(x) == len(f(x))");
            }
            echo_output(m, xi, yi, use_fx);

            outlab02(m, xi, yi, use_fx);
        }
        else {
            auto rg = xi | std::views::transform(user_func);

#ifdef __cpp_lib_containers_ranges
            const dvec ys { std::from_range, rg };
#else
            const dvec ys { rg.begin(), rg.end() };
#endif

            echo_output(m, xi, ys, use_fx);

            outlab02(m, xi, ys, use_fx);
        }
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}