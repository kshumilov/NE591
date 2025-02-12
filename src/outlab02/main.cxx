#include <ranges>
#include <format>
#include <argparse/argparse.hpp>
#include <fstream>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/color.h>

#include "methods/interpolate.h"

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


struct Parameters {
    int n {};
    int m {};
    std::vector<double> x {};
    std::vector<double> y {};
    bool use_user_func { false };

    void validate() const {
        if (x.size() < 2U) {
            throw std::invalid_argument("At least two interpolations points must be provided: |{x_i}| >= 2");
        }

        if (not std::ranges::is_sorted(x)) {
            throw std::invalid_argument("Interpolation points must be sorted in increasing order: x_{i} < x_{i + 1}");
        }

        if (x.size() != y.size()) {
            throw std::invalid_argument("len({x_i}) != len({y_i})");
        }

        if (m < 1) {
            throw std::invalid_argument("Number of samples points must be larger than one: m >= 1");
        }
    }

    /**
     * \brief Echoes back user input in a formated way
     */
    auto print() const -> void {
        fmt::println("{:=^80s}", "");
        fmt::println("{: ^80s}", "Input Arguments");
        fmt::println("{:-^80s}", "");
        fmt::println("#samples: m = {:d}", this->m);
        fmt::println("#points : n = {:d}", this->x.size());
        fmt::println("user-defined function: {}", this->use_user_func);
        fmt::println("{:-^80s}", "");
        fmt::println("{: ^80s}", "Interpolation Points");
        fmt::println("{:-^80s}", "");
        fmt::println("{: ^4s}{: ^38s}{: ^38s}", "i", "x", "f(x)");
        fmt::println("{:-^80s}", "");

        for (const auto [i, x_i, y_i] : std::views::zip(std::views::iota(1), this->x, this->y)) {
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
};


auto parse_and_validate(int argc, char* argv[]) -> std::tuple<Parameters, std::optional<std::string>, std::optional<std::string>> {
    argparse::ArgumentParser program {
        "shumilov_outlab02",
        "1.0",
        argparse::default_arguments::help,
    };

    // program.add_description(description);

    program.add_argument("-n")
        .help("Number of interpolation points.\n"
              "If `-x`, `-y` or `--input` are provided, takes first `n` points.")
        .nargs(1)
        .scan<'d', int>();

    program.add_argument("-m", "--samples")
        .help("Number of samples to interpolate the function at")
        .nargs(1)
        .scan<'d', int>();

    program.add_argument("-x", "--points")
        .help("Distinct real interpolation points in increasing order: {x_i}")
        .append()
        .nargs(argparse::nargs_pattern::at_least_one)
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

    program.add_argument("--interactive")
        .help("Toggle interactive mode.\n"
              "In this mode, the user is queried to enter missing values")
        .flag();

    program.add_argument("--save-input")
        .help("Save the input to the file in csv format")
        .nargs(1);

    program.add_argument("--output")
        .help("Output filename (writes in csv format)")
        .nargs(1);

    try {
        program.parse_args(argc, argv);

        const bool interactive_mode = program.get<bool>("--interactive");

        Parameters params {};

        const std::optional<int> n = program.present<int>("-n");
        const std::optional<int> m = program.present<int>("-m");
        const std::optional<dvec> x = program.present<dvec>("-x");
        const std::optional<dvec> y = program.present<dvec>("-y");

        const bool use_user_func = program.get<bool>("--user-func");

        bool ask_for_n = false;
        bool ask_for_x = false;
        bool ask_for_y = false;
        bool ask_for_m = false;

        // Figure out n and x
        if (n.has_value()) {
            params.n = n.value();

            if (x.has_value()) {
                params.n = std::min(params.n, static_cast<int>(x->size()));

                params.x.clear();
                params.x.reserve(params.n);
                std::copy_n(x->cbegin(), params.n, std::back_inserter(params.x));
            }
            else if (interactive_mode) {
                ask_for_x = true;
            }
            else {
                throw std::runtime_error("Must specify `-x` or `--interactive` to provide interpolated points");
            }
        }
        else {
            if (x.has_value()) {
                params.n = static_cast<int>(x->size());

                params.x.clear();
                params.x.reserve(params.n);
                std::copy_n(x->cbegin(), params.n, std::back_inserter(params.x));
            }
            else if (interactive_mode) {
                ask_for_n = true;
                ask_for_x = true;
            }
            else {
                throw std::runtime_error("Must specify `-x` or `--interactive` to provide interpolated points");
            }
        }

        if (y.has_value() && not use_user_func) {
            params.y.clear();
            params.y.reserve(y->size());
            std::copy_n(y->cbegin(), y->size(), std::back_inserter(params.y));
        }
        else if (interactive_mode && not use_user_func) {
            ask_for_y = true;
        }
        else if (not use_user_func) {
            throw std::runtime_error("Must specify `-y`, `--interactive`, or `--user-func`\n"
                                     "to provide values of interpolated points");
        }

        // Figure out m
        if (m.has_value()) {
            params.m = m.value();
        }
        else if (interactive_mode) {
            ask_for_m = true;
        }
        else {
            throw std::runtime_error("Must specify `-m` or `--interactive` to number of samples");
        }

        if (ask_for_n) {
            std::cout << "Enter number of interpolation points:\n";
            if (std::cin >> params.n) {}
            else {
                throw std::invalid_argument("Invalid input");
            }
        }

        if (ask_for_m) {
            std::cout << "Enter number of sampled points:\n";
            if (std::cin >> params.m) {}
            else {
                throw std::invalid_argument("Invalid input");
            }
        }

        if (ask_for_x) {
            std::cout << fmt::format("Enter {:d} interpolation points:\n", params.n);

            params.x.clear();
            params.x.reserve(params.n);

            double num;
            for (const int i : std::views::iota(0, params.n)) {
                if (std::cin >> num) {
                    params.x.push_back(num);
                }
                else {
                    throw std::invalid_argument("Invalid input");
                }
            }
        }

        if (ask_for_y) {
            std::cout << fmt::format("Enter {:d} values of interpolation points:\n", params.n);

            params.y.clear();
            params.y.reserve(params.n);

            double num;
            for (const int i : std::views::iota(0, params.n))
                if (std::cin >> num) {
                    params.y.push_back(num);
                }
                else {
                    throw std::invalid_argument("Invalid input");
                }
        }
        else if (use_user_func) {
            auto rg = params.x | std::views::transform(user_func);
            params.y = { rg.cbegin(), rg.cend() };
            params.use_user_func = true;
        }

        params.validate();

        return { params, program.present("--save-input"), program.present("--output") };
    }
    catch (const std::exception& err) {
        std::cerr << "\n"
          << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
          << err.what() << "\n\n";
        std::cerr << program.help().str() << "\n";
        std::exit(EXIT_FAILURE);
    }
}


auto outlab02(const Parameters& params) -> std::map<std::string, std::vector<double>> {
    std::map<std::string, std::vector<double>> data {};

    try {
        LagrangeInterpolation<double> lip { params.x, params.y };
        const auto [x, Lx] = lip.sample(params.m);

        data.emplace("x", x);
        data.emplace("L(x)", Lx);
        if (params.use_user_func) {
            auto rgf = x | std::views::transform(user_func);
            const dvec fx { rgf.cbegin(), rgf.cend() };

            auto error = [&](auto f, auto L) { return f - L; };
            auto rge = std::views::zip_transform(error, fx, Lx);
            const dvec Ex { rge.cbegin(), rge.cend() };

            data.emplace("f(x)", fx);
            data.emplace("E(x)", Ex);
        }
    } catch (const std::exception& err) {
        std::cerr << "\n"
                  << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
                  << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

    return data;
}


auto print_output(const std::map<std::string, dvec>& data) -> void {
    const auto headers = data | std::views::keys | std::views::reverse;

    fmt::println("{: ^80s}", "Results");
    fmt::println("{:-^80s}", "");
    fmt::println("{: ^4s}{: ^19s}", "i", fmt::join(headers, ""));
    fmt::println("{:-^80s}", "");

    const auto samples = data.at("x").size();

    for (const std::size_t& i : std::views::iota(0U, samples)) {
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


template<std::floating_point T>
auto save_csv(const std::string& filename, const std::map<std::string, std::vector<T>>& data) -> void {
    std::ofstream outfile{filename};

    const auto headers = data | std::views::keys | std::views::reverse;

    outfile << fmt::format("{:s}", fmt::join(headers, ",")) << '\n';

    const auto max_rows = std::ranges::max(
        data
        | std::views::values
        | std::views::transform(
            [&](const auto& col) -> std::size_t {
                return col.size();
            })
    );

    std::vector<std::string> row {};
    row.reserve(headers.size());

    for (const std::size_t& row_idx : std::views::iota(0U, max_rows)) {
        for (const auto& header : headers) {
            if (row_idx < data.at(header).size()) {
                row.push_back(fmt::format("{:.12e}", data.at(header)[row_idx]));
            } else {
                row.push_back("");
            }
        }

        outfile << fmt::format("{:s}", fmt::join(row, ",")) << '\n';
        row.clear();
    }

    outfile.close();
}


auto main(int argc, char* argv[]) -> int {
    using namespace std::literals;

    std::cout << description;
    const auto [params, input_filename, output_filename] = parse_and_validate(argc, argv);
    params.print();

    if (input_filename.has_value()) {
        std::map<std::string, dvec> data {};
        data.emplace("x", params.x);
        data.emplace("y", params.y);
        save_csv(input_filename.value(), data);
    }

    const auto result = outlab02(params);

    print_output(result);

    if (output_filename.has_value()) {
        save_csv(output_filename.value(), result);
    }

    return EXIT_SUCCESS;
}