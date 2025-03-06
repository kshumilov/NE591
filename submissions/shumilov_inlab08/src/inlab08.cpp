#include <concepts>
#include <cstdlib>
#include <memory>
#include <chrono>
#include <filesystem>

#include <nlohmann/json.hpp>
#include <fmt/color.h>
#include <fmt/ostream.h>
#include <argparse/argparse.hpp>

#include "methods/array.h"
#include "methods/legendre.h"
#include "methods/optimize.h"
#include "methods/linalg/matrix.h"
#include "methods/utils/math.h"
#include "methods/utils/io.h"


using real = long double;
using json = nlohmann::json;
constexpr int columnwidth = 80;
const static std::string eq_divider(columnwidth, '=');
const static std::string dash_divider(columnwidth, '-');


struct Grid
{
    static constexpr int MIN_SPATIAL_CELLS{ 1 };
    static constexpr int MIN_ANGULAR{ 2 };

    int num_angular_points{ MIN_ANGULAR };
    int num_spatial_cells{ MIN_SPATIAL_CELLS };

    [[nodiscard]]
    constexpr Grid() = default;


    [[nodiscard]]
    constexpr Grid(const int N, const int I)
        : num_angular_points{ N }
      , num_spatial_cells{ I }
    {
        validate();
    }


    constexpr auto validate() const -> void
    {
        if (num_spatial_cells < MIN_SPATIAL_CELLS)
            throw std::invalid_argument(
                fmt::format("#spatial cells must be at least {:d}: {:d}", MIN_SPATIAL_CELLS, num_spatial_cells)
            );

        if (num_angular_points < MIN_ANGULAR)
            throw std::invalid_argument(
                fmt::format("#angular points must be at least {:d}: {:d}", MIN_ANGULAR, num_angular_points)
            );
    }


    template<std::floating_point T>
    [[nodiscard]]
    constexpr auto angular_quadrature
    (const FixedPointIterSettings<T>& settings = FixedPointIterSettings<T>{ 1e-12, 5 }) const
    {
        return gauss_legendre_quadrature<T>(num_angular_points, settings);
    }


    template<std::floating_point T>
    constexpr auto spatial_points(const T length)
    {
        return linspace<T>(T{}, length, num_spatial_points());
    }


    [[nodiscard]]
    constexpr auto num_spatial_points() const noexcept
    {
        return num_spatial_cells + 1;
    }


    [[nodiscard]]
    constexpr auto size() const noexcept
    {
        return num_angular_points * num_spatial_cells;
    }


    [[nodiscard]]
    constexpr auto to_string(const int label_width = 40) const
    {
        return fmt::format(
            "Slab Grid:\n"
            "\t{:{}s}: {:d}\n"
            "\t{:{}s}: {:d}",
            "Number of angular grid points, N",
            label_width,
            num_angular_points,
            "Number of spatial cells, I",
            label_width,
            num_spatial_cells
        );
    }


    [[nodiscard]]
    static auto from_file(std::istream& input) -> Grid
    {
        const Grid grid{
            read_positive_value<int>(input, "Number of angular points") * 2,
            read_positive_value<int>(input, "Number of spatial points"),
        };

        grid.validate();

        return grid;
    }


    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Grid, num_spatial_cells, num_angular_points)
};


template<std::floating_point T>
struct Slab
{
    Grid grid{};
    T total_cross_section{ 1 };
    T scattering_cross_section{ 1 };
    T source{ 1 };
    T slab_width{ 1 };


    constexpr auto validate() const -> void
    {
        grid.validate();

        if (total_cross_section < 0)
        {
            throw std::invalid_argument(
                fmt::format("Total cros section must be non-negative: {: 14.6e}", total_cross_section)
            );
        }

        if (scattering_cross_section < 0)
        {
            throw std::invalid_argument(
                fmt::format("Scattering cross section must be non-negative: {: 14.6e}", scattering_cross_section)
            );
        }

        if (scattering_cross_section > total_cross_section)
        {
            throw std::invalid_argument(
                fmt::format(
                    "Total cross section must be larger than scattering cross section: {} < {}",
                    total_cross_section,
                    scattering_cross_section
                )
            );
        }

        if (source < 0)
        {
            throw std::invalid_argument(
                fmt::format("Fixed source must be non-negative: {: 14.6e}", source)
            );
        }

        if (slab_width <= 0)
        {
            throw std::invalid_argument(fmt::format("Slab width must be positive: {}", slab_width));
        }
    }


    [[nodiscard]]
    constexpr auto to_string(const int label_width = 40) const
    {
        return fmt::format(
            "Slab:\n"
            "{:s}\n"
            "Medium:\n"
            "\t{:{}s}: {: 14.6e}\n"
            "\t{:{}s}: {: 14.6e}\n"
            "\t{:{}s}: {: 14.6e}\n"
            "\t{:{}s}: {: 14.6e}",
            grid.to_string(),
            "Total Macroscopic Cross Section, Sa",
            label_width,
            total_cross_section,
            "Scattering Macroscopic Cross Section, St",
            label_width,
            scattering_cross_section,
            "Uniform Fixed Source Strength, q",
            label_width,
            source,
            "Slab Width, L",
            label_width,
            slab_width
        );
    }


    [[nodiscard]]
    auto init_scalar_flux() const -> std::vector<T>
    {
        return std::vector<T>(grid.num_spatial_cells, T{});
    }


    [[nodiscard]]
    auto init_point_angular_flux() const -> Matrix<T>
    {
        const auto rows = static_cast<std::size_t>(grid.num_angular_points);
        const auto cols = static_cast<std::size_t>(grid.num_spatial_points());

        return Matrix<T>::zeros(rows, cols);
    }


    [[nodiscard]]
    auto init_cell_angular_flux() const -> Matrix<T>
    {
        const auto rows = static_cast<std::size_t>(grid.num_angular_points);
        const auto cols = static_cast<std::size_t>(grid.num_spatial_cells);
        return Matrix<T>::zeros(rows, cols);
    }


    [[nodiscard]]
    auto delta() const -> T
    {
        return slab_width / static_cast<T>(grid.num_spatial_cells);
    }


    [[nodiscard]]
    static auto from_file(std::istream& input) -> Slab<T>
    {
        const Slab<T> slab{
            Grid::from_file(input),
            read_positive_value<T>(input, "Total Cross section"),
            read_positive_value<T>(input, "Scattering cross section"),
            read_positive_value<T>(input, "Fixed Source"),
            read_positive_value<T>(input, "Slab Width")
        };

        slab.validate();

        return slab;
    }
};

template<std::floating_point T>
struct Flux
{
    std::shared_ptr<Slab<T>> slab;
    std::vector<T> scalar_flux{};
    Matrix<T> point_angular_flux{};
    Matrix<T> cell_angular_flux{};


    [[nodiscard]]
    explicit Flux(const Slab<T>& slab_)
        : slab(std::make_shared<Slab<T>>(slab_))
      , scalar_flux{ slab->init_scalar_flux() }
      , point_angular_flux{ slab->init_point_angular_flux() }
      , cell_angular_flux{ slab->init_cell_angular_flux() }
    {}


    [[nodiscard]]
    constexpr auto to_string() const
    {
        auto result = fmt::format("{:^5s} {:^14s}\n", "i", "flux");
        for (int i{}; i < slab->grid.num_spatial_cells; ++i)
            fmt::format_to(std::back_inserter(result), "{:5d} {: 14.6e}\n", i + 1, scalar_flux.at(i));
        return result;
    }


    Flux(const Flux<T>& flux_) = default;

    Flux(Flux<T>&& flux_) = default;


    auto source_iteration(const FixedPointIterSettings<T>& iter_settings)
    {
        const auto [angular_nodes, angular_weights] = slab->grid.template angular_quadrature<T>();
        std::vector<T> Q(scalar_flux.size());

        auto g = [&](Flux<T>* flux) -> T
        {
            std::transform(
                flux->scalar_flux.cbegin(),
                flux->scalar_flux.cend(),
                Q.begin(),
                [&](const auto v)
                {
                    return flux->slab->scattering_cross_section * v + flux->slab->source;
                }
            );

            // Forward Sweep, x = 0 -> x = L, mu > 0
            for (int n = flux->slab->grid.num_angular_points / 2; n < flux->slab->grid.num_angular_points; ++n)
            {
                const auto two_mu_over_delta = 2 * angular_nodes.at(n) / slab->delta();
                assert(two_mu_over_delta > 0);
                for (int i{}; i < flux->slab->grid.num_spatial_cells; ++i)
                {
                    flux->cell_angular_flux.at(n, i) =
                        (Q.at(i) + two_mu_over_delta * flux->point_angular_flux.at(n, i)) / (
                            two_mu_over_delta + flux->slab->total_cross_section);

                    flux->point_angular_flux.at(n, i + 1) =
                        2 * flux->cell_angular_flux.at(n, i) - flux->point_angular_flux.at(n, i);
                }
            }

            // Backward Sweep, x = L -> x = 0, mu < 0
            for (int n{}; n < flux->slab->grid.num_angular_points / 2; ++n)
            {
                const auto two_mu_over_delta = -2 * angular_nodes.at(n) / flux->slab->delta();
                assert(two_mu_over_delta > 0);
                for (int i = flux->slab->grid.num_spatial_cells - 1; i > -1; --i)
                {
                    flux->cell_angular_flux.at(n, i) =
                        (Q.at(i) + two_mu_over_delta * flux->point_angular_flux.at(n, i + 1)) / (
                            two_mu_over_delta + flux->slab->total_cross_section);

                    flux->point_angular_flux.at(n, i) =
                        2 * flux->cell_angular_flux.at(n, i) - flux->point_angular_flux.at(n, i + 1);
                }
            }

            // Update Scalar flux
            T max_abs_error{};
            for (int i{}; i < flux->slab->grid.num_spatial_cells; ++i)
            {
                const auto new_value = std::transform_reduce(
                    angular_weights.cbegin(),
                    angular_weights.cend(),
                    flux->cell_angular_flux.col(i).cbegin(),
                    T{},
                    std::plus<T>{},
                    std::multiplies<T>{}
                );

                if (const auto error = rel_diff(new_value, flux->scalar_flux.at(i));
                    error > max_abs_error)
                {
                    max_abs_error = error;
                }

                flux->scalar_flux.at(i) = new_value;
            }

            return max_abs_error;
        };

        return fixed_point_iteration(
            g,
            this,
            iter_settings
        );
    }
};

template<std::floating_point T>
struct Lab08
{
    Slab<T> slab;
    FixedPointIterSettings<T> iter_settings;

    struct Solution
    {
        std::shared_ptr<Lab08> lab;
        Flux<T> flux;
        FixedPointIterResult<Flux<T>*, T> result;
        std::chrono::duration<long long, std::nano> time{}; // nanoseconds

        auto echo(std::ostream& output) const
        {
            lab->echo(output);
            fmt::println(output, "{}\n{}", result.to_string(), dash_divider);
            fmt::println(output, "{}\n{}", flux.to_string(), dash_divider);
            fmt::println(
                output,
                "Execution time: {:%S} seconds.\n{}",
                time,
                eq_divider
            );
        }
    };

    [[nodiscard]]
    auto run() -> Solution
    {
        Flux flux{ slab };
        const auto start = std::chrono::high_resolution_clock::now();
        const auto result = flux.source_iteration(iter_settings);
        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return {
            std::make_shared<Lab08<T>>(*this),
            std::move(flux),
            std::move(result),
            duration
        };
    }

    auto echo(std::ostream& output) const -> void
    {
        fmt::println(
            output,
            "{}\n{}"
            "\n{}\n{}",
            slab.to_string(),
            dash_divider,
            iter_settings.to_string(),
            dash_divider
        );
    }

    [[nodiscard]]
    static auto from_file(std::istream& input) -> Lab08<T>
    {
        return {
            .slab = Slab<T>::from_file(input),
            .iter_settings = FixedPointIterSettings<T>::from_file(input),
        };
    }
};


struct Header
{
    std::string title{ "NE 591 Inlab #08" };
    std::string author{ "Kirill Shumilov" };
    std::string date{ "02/28/2025" };
    std::string description{ "Solving Source Iteration Equations" };


    [[nodiscard]] constexpr auto to_string() const -> std::string
    {
        return fmt::format(
            "{}\n"
            "{:s}\n"
            "Author: {:s}\n"
            "Date: {:s}\n"
            "{}\n"
            "{:s}\n"
            "{}\n",
            eq_divider,
            title,
            author,
            date,
            dash_divider,
            description,
            eq_divider
        );
    }


    void echo(std::ostream& out) const { out << this->to_string(); }
};


template<std::floating_point T>
[[nodiscard]]
auto read_input_file(const std::string& filename) -> Lab08<T>
{
    const auto input_filepath = std::filesystem::path{ filename };

    if (input_filepath.empty())
    {
        throw std::runtime_error(
            fmt::format("Input file does not exist: {}", input_filepath.string())
        ); // Indicate an error occurred
    }

    std::ifstream input{ input_filepath };


    if (!input.is_open())
    {
        throw std::runtime_error(
            fmt::format("Could not open input file: {}", input_filepath.string())
        ); // Indicate an error occurred
    }

    try
    {
        const auto p = Lab08<T>::from_file(input);
        input.close();
        return p;
    }
    catch (const std::exception&)
    {
        input.close();
        throw;
    }
}


auto main(int argc, char* argv[]) -> int
{
    const Header header{};
    argparse::ArgumentParser program{
        "shumilov_inlab08",
        "1.0",
        argparse::default_arguments::help,
    };

    program.add_argument("input").help("Path to input file.");
    program.add_argument("-o", "--output").help("Path to output file");

    try
    {
        program.parse_args(argc, argv);

        auto lab = read_input_file<real>(
            program.get<std::string>("input")
        );
        const auto solution = lab.run();

        if (const auto output_filename = program.present<std::string>("--output");
            output_filename.has_value())
        {
            std::ofstream output{ output_filename.value() };
            if (!output.is_open())
            {
                throw std::runtime_error(
                    fmt::format("Could not open: '{}'", output_filename.value())
                ); // Indicate an error occurred
            }

            header.echo(output);
            solution.echo(output);

            output.close();
        }
        else {
            header.echo(std::cout);
            solution.echo(std::cout);
        }
    }
    catch (const std::exception& err)
    {
        fmt::print(
            std::cerr,
            "\n{}: {}\n\n",
            fmt::format(
                fmt::emphasis::bold | fg(fmt::color::red),
                "Error: "
            ),
            err.what()
        );
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
