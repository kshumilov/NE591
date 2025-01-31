#include <cmath>
#include <cstddef>
#include <vector>
#include <ranges>

#include <fmt/format.h>
#include <fmt/color.h>

#include <argparse/argparse.hpp>

#include "integrate/integrate.h"
#include "user_func.h"


template<>
struct fmt::formatter<Quadrature, char> {
    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<class FmtContext>
    constexpr auto format(const Quadrature q, FmtContext& ctx) const
    {
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


/**
 *
 * @param str string to convert to Quadrature enum
 * @return optional associated with Quadrature type
 */
auto str_to_quadrature(const std::string_view str) noexcept -> std::optional<Quadrature>
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

/**
 * @brief Struct to hold Outlab03 Inputs
 */
struct Outlab03 {
    double start{}; /** Start of integration interval */
    double end{}; /** Start of integration interval */
    int subintervals{}; /** Number of intervals to divide [a,b] into, must be non-positive */
    Quadrature quadrature;  /** Quadrature type */

    double result{};

    /**
     * @brief Subinterval size
     */
    [[nodiscard]]
    constexpr auto step() const -> double
    {
        return subdivide(start, end, subintervals);
    }

    [[nodiscard]]
    constexpr auto l() const -> std::size_t
    {
        return static_cast<std::size_t>(points());
    }

    auto run(int argc, char* argv[]) -> void
    {
        std::cout <<
            "================================================================================\n"
            "NE 591 Outlab #03: Numerical Integration\n"
            "Author: Kirill Shumilov\n"
            "Date: 01/31/2025\n"
            "================================================================================\n"
            "This program performs composite numerical integration of user-defined function\n"
            "using three quadratures: Trapezoidal, Simpson, and Gauss\n";


        parse_and_validate(argc, argv);
        print_input();

        if (quadrature == Quadrature::Gauss) {
            print_gauss_nodes();
        }

        try {
            result = integrate<double>(user_func, start, end, points(), quadrature);
        }
        catch (const std::exception& err) {
            std::cerr << "\n"
                    << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
                    << err.what() << "\n\n";
            std::exit(EXIT_FAILURE);
        }

        print_results();
    }

    /**
     * @brief Number of Quadrature points
     */
    [[nodiscard]]
    constexpr auto points() const noexcept-> int
    {
        return subintervals + 1;
    }

    auto parse_and_validate(int argc, char* argv[]) -> void
    {
        argparse::ArgumentParser program{
            "shumilov_outlab03",
            "1.0",
            argparse::default_arguments::help,
        };

        program.add_argument("-a", "--start")
               .help("Start of interval integration")
               .scan<'g', double>();

        program.add_argument("-b", "--end")
               .help("End of interval integration")
               .scan<'g', double>();

        program.add_argument("-m", "--subintervals")
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

            const auto start_opt = program.present<double>("-a");
            const auto end_opt = program.present<double>("-b");
            const auto subintervals_opt = program.present<int>("-m");
            const auto quad_str_opt = program.present<std::string>("--quad");

            const auto interactive = program.get<bool>("--interactive");

            if (not start_opt.has_value() || interactive) {
                std::cout << "Enter interval start:\n";
                std::cin >> start;
            }
            else if (start_opt.has_value()) {
                start = start_opt.value();
            }
            else {
                throw std::runtime_error("Must specify `-a` or `--interactive` to provide interval start.");
            }

            if (not end_opt.has_value() || interactive) {
                std::cout << "Enter interval end:\n";
                std::cin >> end;
            }
            else if (end_opt.has_value()) {
                end = end_opt.value();
            }
            else {
                throw std::runtime_error("Must specify `-b` or `--interactive` to provide interval end.");
            }

            if (not subintervals_opt.has_value() || interactive) {
                std::cout << "Enter interval number of intervals:\n";
                std::cin >> subintervals;
            }
            else if (subintervals_opt.has_value()) {
                subintervals = subintervals_opt.value();
            }
            else {
                throw std::runtime_error("Must specify `-m` or `--interactive` to provide number of subintervals.");
            }

            if (subintervals < 1) {
                throw std::runtime_error(
                    fmt::format("Number of subintervals must be positive: m({}) < 1", subintervals)
                );
            }

            if (not quad_str_opt.has_value() || interactive) {
                int quad_int{};
                std::cout << "Enter type of quadrature 0/1/2 (Trapezoidal/Simpson/Gauss):\n";
                std::cin >> quad_int;
                if (quad_int < 0 || 2 < quad_int) {
                    throw std::runtime_error("Unknown quadrature");
                }
                quadrature = Quadrature{quad_int};
            }
            else if (const auto q = str_to_quadrature(quad_str_opt.value())) {
                quadrature = q.value();
            }
            else {
                throw std::runtime_error("Must specify `--quad` or `--interactive` to provide quadrature.");
            }

            if (quadrature == Quadrature::Simpson && subintervals % 2 == 1) {
                throw std::logic_error("Simpson quadrature must have even number of subintervals.");
            }
        }
        catch (const std::exception& err) {
            std::cerr << "\n"
                    << fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ")
                    << err.what() << "\n\n";
            std::cerr << program.help().str() << "\n";
            std::exit(EXIT_FAILURE);
        }
    }

    auto print_input() const -> void
    {
        fmt::println("{:=^80s}", "");
        fmt::println("{: ^80s}", "Input Arguments");
        fmt::println("{:-^80s}", "");
        fmt::println("Integration Interval...: [{:g}, {:g}]", start, end);
        fmt::println("Integration Step.......: {:g}", step());
        fmt::println("Number of Subintervals.: {:d}", subintervals);
        fmt::println("Number of Points       : {:d}", points());
        fmt::println("Quadrature.............: {}", quadrature);
        fmt::println("{:=^80s}", "");
    }

    auto print_gauss_nodes() const -> void
    {
        fmt::println("{: ^80s}", "Gauss Nodes");
        fmt::println("{:-^80s}", "");
        fmt::println("{: ^4s}{: ^19s}{: ^19s}", "i", "x_i", "w_i");
        fmt::println("{:-^80s}", "");

        for (const std::size_t& k: std::views::iota(1U, l() + 1U)) {
            fmt::print("{: <4d}", k);
            const auto r = legendre_root<double>(l(), k);
            fmt::print("{: >19.11e}", r);
            fmt::print("{: >19.11e}", gauss_weight(l(), r));
            std::cout << "\n";
        }
        fmt::println("{:=^80s}", "");
    }

    auto print_results() const -> void
    {
        fmt::println("{: ^80s}", "Results");
        fmt::println("{:-^80s}", "");
        fmt::println("Integration Interval...: [{:g}, {:g}]", start, end);
        fmt::println("Integration Step.......: {:g}", step());
        fmt::println("Number of Subintervals.: {:d}", subintervals);
        fmt::println("Quadrature.............: {}", quadrature);
        fmt::println("Integral...............: {:.12f}", result);
        fmt::println("{:=^80s}", "");
    }
};


void test()
{
    const int points = 5;
    const double a = -1;
    const double b = 1;

    const auto trap = integrate<double>(user_func, a, b, points, Quadrature::Trapezoidal);
    fmt::println("Trapezoidal: {}", trap);

    const auto simp = integrate<double>(user_func, a, b, points, Quadrature::Simpson);
    fmt::println("Simpson    : {}", simp);

    const auto gaus = integrate<double>(user_func, a, b, points, Quadrature::Gauss);
    fmt::println("Gauss      : {}", gaus);
    fmt::println("True       : {}", user_func(b) - user_func(a));
}

auto main(int argc, char* argv[]) -> int
{
    Outlab03 lab {};
    lab.run(argc, argv);
    return EXIT_SUCCESS;
}