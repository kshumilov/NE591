#include <ranges>
#include <format>
#include <argparse/argparse.hpp>
#include <fstream>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "csv2/csv2.hpp"

#include "interpolate.h"

#include "user_func.h"

using namespace std::literals;
using dvec = std::vector<double>;


const std::string description {
        "================================================================================\n"
        "NE 591 OutLab #02: Lagrange Interpolation\n"
        "Author: Kirill Shumilov\n"
        "Date: 01/23/2025\n"
        "================================================================================\n"
        "This program perform Lagrange Interpolation of a 1D real function\n"
};


auto parse_and_validate(int argc, char* argv[]) -> std::tuple<int, dvec, dvec, bool, std::optional<std::string>> {
    argparse::ArgumentParser program {
        "shumilov_inlab02",
        "1.0",
        argparse::default_arguments::help,
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
        .flag();

    program.add_argument("--output")
        .help("Output filename (writes in csv format)")
        .nargs(1);

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

        if (use_user_func) {
            auto rg = xi | std::views::transform(user_func);
            const dvec yi { rg.cbegin(), rg.cend() };
            return { m, xi, yi, use_user_func, program.present("--output") };
        }

        const auto yi = program.get<dvec>("-y");
        if (xi.size() != yi.size()) {
            throw std::invalid_argument("len({x_i}) != len({y_i})");
        }
        return { m, xi, yi, use_user_func, program.present("--output") };
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(EXIT_FAILURE);
    }
}

/**
 * \brief Echoes back user input in a formated way
 *
 * \param samples number of samples
 * \param x vector with interpolated points
 * \param y vector with values at interpolated points
 */
auto print_input(const int samples, const dvec& x, const dvec& y, const bool use_user_function) -> void {
    fmt::println("{:=^80s}", "");
    fmt::println("{: ^80s}", "Input Arguments");
    fmt::println("{:-^80s}", "");
    fmt::println("#samples: m = {:d}", samples);
    fmt::println("#points : n = {:d}", x.size());
    fmt::println("user-defined function: {}", use_user_function);
    fmt::println("{:-^80s}", "");
    fmt::println("{: ^80s}", "Interpolation Points");
    fmt::println("{:-^80s}", "");
    fmt::println("{: ^4s}{: ^38s}{: ^38s}", "i", "x", "f(x)");
    fmt::println("{:-^80s}", "");

    for (const auto [i, x_i, y_i] : std::views::zip(std::views::iota(1), x, y)) {
        fmt::print("{: >4d}", i);
        fmt::print("{: >38.12E}", x_i);
        fmt::print("{: >28.12E}", y_i);
        std::cout << "\n";
    }
    fmt::println("{:-^80s}", "");
    fmt::println("Where");
    fmt::println("i    : index of the interpolated point");
    fmt::println("x    : position of the interpolated point");
    fmt::println("f(x) : either user-supplied y-values or values from y = f(x) from the function");
    fmt::println("{:=^80s}", "");
}


auto print_output(const int samples, const std::map<std::string, dvec>& data) -> void {
    const auto headers = data | std::views::keys | std::views::reverse;

    fmt::println("{: ^80s}", "Results");
    fmt::println("{:-^80s}", "");
    fmt::println("{: ^4s}{: ^19s}", "i", fmt::join(headers, ""));
    fmt::println("{:-^80s}", "");

    for (const std::size_t& i : std::views::iota(0, samples)) {
        fmt::print("{: <4d}", i + 1);
        for (const std::string& header : headers) {
            fmt::print("{: >19.11e}", data.at(header)[i]);
        }
        std::cout << "\n";
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


auto save_csv(const std::string& filename, const int samples, const std::map<std::string, dvec>& data) -> void {
    std::ofstream outfile{filename};
    csv2::Writer<csv2::delimiter<','>> writer{outfile};

    const auto headers = data | std::views::keys | std::views::reverse;

    std::vector<std::string> row {};
    row.insert(row.end(), headers.begin(), headers.end());
    writer.write_row(row);
    row.clear();

    for (const std::size_t& i : std::views::iota(0, samples)) {
        for (const std::string& header : headers) {
            row.emplace_back(fmt::format("{:.12e}", data.at(header)[i]));
        }
        writer.write_row(row);
        row.clear();
    }

    outfile.close();
}


auto main(int argc, char* argv[]) -> int {
    using namespace std::literals;

    std::cout << description;
    const auto [samples, xi, yi, use_user_func, output] = parse_and_validate(argc, argv);
    print_input(samples, xi, yi, use_user_func);

    std::map<std::string, std::vector<double>> data {};

    try {
        LagrangeInterpolation<double> lip { xi, yi };
        const auto [x, Lx] = lip.sample(samples);

        data.emplace("x", x);
        data.emplace("L(x)", Lx);
        if (use_user_func) {
            auto rgf = x | std::views::transform(user_func);
            const dvec fx { rgf.cbegin(), rgf.cend() };

            auto error = [&](auto f, auto L) { return f - L; };
            auto rge = std::views::zip_transform(error, fx, Lx);
            const dvec EX { rge.cbegin(), rge.cend() };

            data.emplace("f(x)", fx);
            data.emplace("E(x)", EX);
        }
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    print_output(samples, data);

    if (output.has_value()) {
        save_csv(output.value(), samples, data);
    }

    return EXIT_SUCCESS;
}