#include <ranges>
#include <format>
#include <argparse/argparse.hpp>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "interpolate.h"
#include "array.h"

#include "user_func.h"


using dvec = std::vector<double>;

/**
 * \brief Calculates the error between true and interpolated points, based on user-defined function.
 *
 * @param xs vector of sampled points, x
 * @param Ls values of Lagrange Interpolation at sampled points, L(x)
 *
 * @return Error between the true f(x) and L(x)
 */
auto interpolation_error(const dvec& xs, const dvec& Ls) -> std::pair<dvec, dvec> {
    auto rgf = xs | std::views::transform(user_func);
    const dvec fs {rgf.cbegin(), rgf.cend()};

    auto error = [&](auto f, auto L) { return f - L; };
    auto rge = std::views::zip_transform(error, fs, Ls);
    const dvec Es {rge.cbegin(), rge.cend()};
    return {fs, Es};
}

/**
 * \brief Evaluates Lagrange Interpolation in the defined interval
 *
 * @param samples number of equidistant sampled to evaluate L(x) in the intreval [min(xi), max(xi)]
 * @param xi Vector of interpolated points, x_i
 * @param yi Vector of values of function at interpolated points, y_i = f(x_i)
 *
 * @return A pair of vectors, where the first vector is {x_n} - sampled points
 */
auto interpolate(const int samples, const dvec& xi, const dvec& yi) -> std::pair<dvec, dvec> {
    const LagrangeInterpolation<double> inter {xi, yi};
    auto const xs = linspace(xi.front(), xi.back(), samples);
    return {xs, inter(xs)};
}

/**
 * \brief Echoes back user input in a formated way
 */
auto echo_input(const int m, const dvec& xs, const dvec& ys, const bool user_user_function) -> void {
    fmt::println("{:=^80s}", "");
    fmt::println("{: ^80s}", "Input Arguments");
    fmt::println("{:-^80s}", "");
    fmt::println("#samples: m = {:d}", m);
    fmt::println("#points : n = {:d}", xs.size());
    fmt::println("user-defined function: {}", user_user_function);
    fmt::println("{:-^80s}", "");
    fmt::println("{: ^80s}", "Interpolation Points");
    fmt::println("{:-^80s}", "");
    fmt::println("{: ^4s}{: ^38s}{: ^38s}", "i", "x", "f(x)");
    fmt::println("{:-^80s}", "");

    for (const auto [i, x, y] : std::views::zip(std::views::iota(1), xs, ys)) {
        fmt::print("{: >4d}", i);
        fmt::print("{: >38.12E}", x);
        fmt::print("{: >28.12E}", y);
        std::cout << "\n";
    }
    fmt::println("{:-^80s}", "");
    fmt::println("Where");
    fmt::println("i    : index of the interpolated point");
    fmt::println("x    : position of the interpolated point");
    fmt::println("f(x) : either user-supplied y-values or values from y = f(x) from the function");
    fmt::println( "{:=^80s}", "");
}


auto main(int argc, char* argv[]) -> int {
    using namespace std::literals;

    std::unordered_map<std::string, std::vector<double>> input_table_data {};

    std::vector<std::string> output_table_headers {};
    std::unordered_map<std::string, std::vector<double>> output_table_data {};

    argparse::ArgumentParser program {
        "shumilov_inlab02",
        "1.0",
        argparse::default_arguments::help,
    };

    const std::string description {
        "================================================================================\n"s +
        std::format("{:<80s}\n", "NE 591 Inlab #02: Lagrange Interpolation I/O") +
        std::format("{:<80s}\n", "Author: Kirill Shumilov") +
        std::format("{:<80s}\n", "Date: 01/17/2025") +
        "================================================================================\n"s
        "This program perform Lagrange Interpolation of a 1D real function\n"s
      };

    program.add_description(description);

    program.add_argument("-n")
        .help("Number of interpolation points")
        .scan<'d', int>();

    program.add_argument("-m", "--samples")
        .help("Number of samples to interpolate the function at")
        .required()
        .scan<'d', int>();

    program.add_argument("-x", "--points")
        .help("Distinct real interpolation points in increasing order: {x_i}")
        .append()
        .nargs(argparse::nargs_pattern::at_least_one)
        .required()
        .scan<'g', double>();

    program.add_argument("-y", "--values")
        .help("Function values at interpolation points, y_i = f(x_i).\n"
              "Ignored when `--user-func` is provided")
        .append()
        .nargs(argparse::nargs_pattern::at_least_one)
        .scan<'g', double>();

    program.add_argument("--user-func")
        .help("Toggle the use of user-defined function in user_func.h")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
        const auto xi = program.get<dvec>("-x");

        if (xi.size() < 2U) {
            throw std::invalid_argument("At least two interpolations points must be provided: |{x_i}| >= 2");
        }

        if (not std::ranges::is_sorted(xi)) {
            throw std::invalid_argument("Interpolation points must be sorted in increasing order: x_{i} < x_{i + 1}");
        }

        auto m = program.get<int>("-m");
        if (m < 1) {
            throw std::invalid_argument("Number of samples points must be larger than one: m > 1");
        }

        auto use_user_func = program.get<bool>("--user-func");


        if (not use_user_func) {
            const auto yi = program.get<dvec>("-y");
            if (xi.size() != yi.size()) {
                throw std::invalid_argument("len({x_i}) == len({y_i})");
            }

            input_table_data.emplace("y", std::move(yi));
        }
        else {
            auto rg = xi | std::views::transform(user_func);
            const dvec yi { rg.cbegin(), rg.cend() };
            input_table_data.emplace("y", std::move(yi));
        }

        input_table_data.emplace("x", std::move(xi));

        std::cout << description;
        echo_input(m, input_table_data.at("x"), input_table_data.at("y"), use_user_func);

        const auto [xs, Ls] = interpolate(m, xi, input_table_data.at("y"));

        output_table_data.emplace("x", std::move(xs));
        output_table_headers.emplace_back("x");

        output_table_data.emplace("L(x)", std::move(Ls));
        output_table_headers.emplace_back("L(x)");

        if (use_user_func) {
            const auto [fs, Es] = interpolation_error(xs, Ls);
            output_table_data.emplace("f(x)", std::move(fs));
            output_table_headers.emplace_back("f(x)");

            output_table_data.emplace("E(x)", std::move(Es));
            output_table_headers.emplace_back("E(x)");
        }

        std::cout << std::format("{: ^80s}\n", "Results")
                  << std::format("{:-^80s}\n", "")
                  << fmt::format("{: ^4s}{: ^19s}\n", "i", fmt::join(output_table_headers, ""))
                  << std::format("{:-^80s}\n", "");

        for (const std::size_t& i : std::views::iota(0, m)) {
            std::cout << fmt::format("{: <4d}", i + 1);
            for (const std::string& header : output_table_headers) {
                std::cout << fmt::format("{: >19.11e}", output_table_data.at(header)[i]);
            }
            std::cout << std::endl;
        }
        fmt::println("{:-^80s}", "");
        fmt::println("Where");
        fmt::println("i    : index of the sampled point");
        fmt::println("x    : position of the sampled point");
        fmt::println("L(x) : interpolated value at x");
        fmt::println("f(x) : True value, based on user-defined function");
        fmt::println("E(x) : L(x) - f(x)");
        fmt::println( "{:=^80s}", "");
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}