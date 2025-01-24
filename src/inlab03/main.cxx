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
                return fmt::format_to(ctx.out(), "Gauss");
        }
        return ctx.out();
    }
};

auto str_to_quadrature(std::string_view str) -> std::optional<Quadrature>
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
    std::size_t subintervals {};
    Quadrature quadrature;

    [[nodiscard]]
    constexpr auto step() const -> double
    {
        return (end - start) / subintervals;
    }

    [[nodiscard]]
    constexpr auto points() const -> std::size_t
    {
        return subintervals + 1U;
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
           .required()
           .scan<'g', real>();

    program.add_argument("-b", "--end")
           .help("End of interval integration")
           .required()
           .scan<'g', real>();

    program.add_argument("-m","--subintervals")
           .help("Number of subintervals")
           .required()
           .scan<'d', int>();

    program.add_argument("--quad")
           .help("Quadrature type")
           .default_value(std::string{"trap"})
           .choices("trap", "simp", "gauss");

    try {
        program.parse_args(argc, argv);
        const double start = program.get<real>("-a");
        const double end = program.get<real>("-b");

        const auto subintervals = program.get<int>("-m");
        if (subintervals < 1) {
            throw std::runtime_error(
                fmt::format("Number of subintervals must be positive: m({}) < 1", subintervals)
            );
        }

        Quadrature quadrature = Quadrature::Trapezoidal;
        if (const auto q = str_to_quadrature(program.get<std::string>("--quad"))) {
            quadrature = q.value();
        } else {
            throw std::runtime_error("Unknown quadrature");
        }

        return {
            .start = start,
            .end = end,
            .subintervals = static_cast<std::size_t>(subintervals),
            .quadrature = quadrature,
        };
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

    #ifndef NDEBUG
    fmt::println("Integrating Points:");
    fmt::println("{: ^12}|{: ^12s}", "x", "f(x)");
    fmt::println("{:-^12s}+{:-^12s}", "", "");
    for (const auto &[xi, yi] : std::views::zip(x, y)) {
        fmt::println("{: >12g}|{: >12g}", xi, yi);
    }
    fmt::println("{:=^80s}", "");
    #endif

    const double integral = integrate<double>(y, inputs.quadrature, inputs.step());

    fmt::println("{: ^80s}", "Results");
    fmt::println("{:-^80s}", "");
    fmt::println("Integration Interval...: [{:g}, {:g}]", inputs.start, inputs.end);
    fmt::println("Integration Step.......: {:g}", inputs.step());
    fmt::println("Number of Subintervals.: {:d}", inputs.subintervals);
    fmt::println("Quadrature.............: {}", inputs.quadrature);
    fmt::println("Integrals..............: {:.12f}", integral);
    fmt::println("{:=^80s}", "");
}