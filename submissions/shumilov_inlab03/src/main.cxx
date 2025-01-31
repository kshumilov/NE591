#include <cstddef>
#include <concepts>
#include <format>
#include <stdexcept>
#include <argparse/argparse.hpp>

#include <fmt/format.h>
#include <fmt/color.h>

#include "integrate.h"
#include "array.h"

#include "user_func.h"


using real = double;


template <>
struct fmt::formatter<Quadrature, char> {
    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<class FmtContext>
    constexpr auto format(Quadrature q, FmtContext& ctx) const {
        switch (q) {
            case Quadrature::Trapezoidal:
                return fmt::format_to(ctx.out(), "Trapezoidal");
            case Quadrature::Simpson:
                return fmt::format_to(ctx.out(), "Simpson");
            case Quadrature::Gauss:
                return fmt::format_to(ctx.out(), "Gauss-Legendre");
        }
        return ctx.out();
    }
};


auto str_to_quadrature(const std::string_view str) -> std::optional<Quadrature>
{
    if (str.starts_with("trap")) {
        return Quadrature::Trapezoidal;
    }

    if (str == "simp") {
        return Quadrature::Simpson;
    }

    if (str == "gauss") {
        return Quadrature::Gauss;
    }
    return std::nullopt;
}


struct Inputs {
    double start {};
    double end {};
    int subintervals {};
    Quadrature quadrature;

    [[nodiscard]]
    constexpr auto step() const -> double
    {
        return (end - start) / subintervals;
    }

    [[nodiscard]]
    constexpr auto points() const -> int
    {
        return subintervals + 1;
    }
};


auto parse_and_validate(int argc, char* argv[]) -> Inputs
{
    argparse::ArgumentParser program{
        "shumilov_inlab03",
        "1.0",
        argparse::default_arguments::help,
    };

    program.add_argument("-a","--start")
           .help("Start of interval integration")
           .scan<'g', real>();

    program.add_argument("-b", "--end")
           .help("End of interval integration")
           .scan<'g', real>();

    program.add_argument("-m","--subintervals")
           .help("Number of subintervals")
           .scan<'d', int>();

    program.add_argument("--quad")
           .help("Quadrature type (trap, simp, gauss)")
           .choices("trap", "simp", "gauss");

    program.add_argument("--interactive")
           .help("Toggle interactive mode")
           .flag();

    try {
        program.parse_args(argc, argv);

        Inputs inputs{};

        const auto start_opt = program.present<real>("-a");
        const auto end_opt = program.present<real>("-b");
        const auto subintervals_opt = program.present<int>("-m");
        const auto quad_str_opt = program.present<std::string>("--quad");

        const auto interactive = program.get<bool>("--interactive");

        if (not start_opt.has_value() || interactive) {
            std::cout << "Enter interval start:\n";
            std::cin >> inputs.start;
        } else if (start_opt.has_value()) {
            inputs.start = start_opt.value();
        } else {
            throw std::runtime_error("Must specify `-a` or `--interactive` to provide interval start.");
        }

        if (not end_opt.has_value() || interactive) {
            std::cout << "Enter interval end:\n";
            std::cin >> inputs.end;
        } else if (end_opt.has_value()) {
            inputs.end = end_opt.value();
        } else {
            throw std::runtime_error("Must specify `-b` or `--interactive` to provide interval end.");
        }

        if (not subintervals_opt.has_value() || interactive) {
            std::cout << "Enter interval number of intervals:\n";
            std::cin >> inputs.subintervals;
        } else if (subintervals_opt.has_value()) {
            inputs.subintervals = subintervals_opt.value();
        } else {
            throw std::runtime_error("Must specify `-m` or `--interactive` to provide number of subintervals.");
        }

        if (inputs.subintervals < 1) {
            throw std::runtime_error(
                fmt::format("Number of subintervals must be positive: m({}) < 1", inputs.subintervals)
            );
        }

        if (not quad_str_opt.has_value() || interactive) {
            int quad_int {};
            std::cout << "Enter type of quadrature 0/1/2 (Trapezoidal/Simpson/Gauss):\n";
            std::cin >> quad_int;
            if (quad_int < 0 || 2 < quad_int) {
                throw std::runtime_error("Unknown quadrature");
            }
            inputs.quadrature = Quadrature{quad_int};
        } else if (const auto q = str_to_quadrature(quad_str_opt.value())) {
            inputs.quadrature = q.value();
        } else {
            throw std::runtime_error("Must specify `--quad` or `--interactive` to provide quadrature.");
        }

        return inputs;
    }
    catch (const std::exception& err) {
        std::cerr << "\n"
          << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
          << err.what() << "\n\n";
        std::cerr << program.help().str() << "\n";
        std::exit(EXIT_FAILURE);
    }
}


auto print_input(const Inputs& inputs) -> void
{
    fmt::println("{:=^80s}", "");
    fmt::println("{: ^80s}", "Input Arguments");
    fmt::println("{:-^80s}", "");
    fmt::println("Integration Interval...: [{:g}, {:g}]", inputs.start, inputs.end);
    fmt::println("Integration Step.......: {:g}", inputs.step());
    fmt::println("Number of Subintervals.: {:d}", inputs.subintervals);
    fmt::println("Quadrature.............: {}", inputs.quadrature);
    fmt::println("{:=^80s}", "");
}


auto main(int argc, char* argv[]) -> int {
    const std::string description{
        "================================================================================\n"
        "NE 591 Inlab #03: Numerical Integration\n"
        "Author: Kirill Shumilov\n"
        "Date: 01/24/2025\n"
        "================================================================================\n"
        "This program performs composite numerical integration of user-defined function\n"
        "using three quadratures: Trapezoidal, Simpson, and Gauss\n"
    };

    std::cout << description;

    const auto inputs = parse_and_validate(argc, argv);
    print_input(inputs);

    const std::vector<double> x = linspace(inputs.start, inputs.end, inputs.points());
    std::vector<double> y {};
    std::ranges::transform(x, std::back_inserter(y), user_func);

    // #ifndef NDEBUG
    // fmt::println("Integrating Points:");
    // fmt::println("{: ^12}|{: ^12s}", "x", "f(x)");
    // fmt::println("{:-^12s}+{:-^12s}", "", "");
    // for (const auto &[xi, yi] : std::views::zip(x, y)) {
    //     fmt::println("{: >12g}|{: >12g}", xi, yi);
    // }
    // fmt::println("{:=^80s}", "");
    // #endif

    try {
        const double integral = integrate<double>(y, inputs.quadrature, inputs.step());

        fmt::println("{: ^80s}", "Results");
        fmt::println("{:-^80s}", "");
        fmt::println("Integration Interval...: [{:g}, {:g}]", inputs.start, inputs.end);
        fmt::println("Integration Step.......: {:g}", inputs.step());
        fmt::println("Number of Subintervals.: {:d}", inputs.subintervals);
        fmt::println("Quadrature.............: {}", inputs.quadrature);
        fmt::println("Integral...............: {:.12f}", integral);
        fmt::println("{:=^80s}", "");
    }
    catch (const std::exception& err) {
        std::cerr << "\n"
          << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
          << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

}