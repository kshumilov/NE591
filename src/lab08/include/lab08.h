#ifndef LAB08_H
#define LAB08_

#include <concepts>
#include <istream>

#include "methods/optimize.h"

#include "slab.h"
#include "utils.h"


struct Info
{
    std::string title{ "NE 591 Inlab #08" };
    std::string author{ "Kirill Shumilov" };
    std::string date{ "02/28/2025" };
    std::string description{ "Solving Source Iteration Equations" };


    [[nodiscard]] constexpr auto to_string() const -> std::string
    {
        return fmt::format(
            "{0:=^{1}}\n"
            "{2:s}\n"
            "Author: {3:s}\n"
            "Date: {4:s}\n"
            "{0:-^{1}}\n"
            "{5:s}\n"
            "{0:=^{1}}\n",
            "", COLUMN_WIDTH,
            title,
            author,
            date,
            description
        );
    }


    void echo(std::ostream& out) const { out << this->to_string(); }
};


template<std::floating_point T>
struct Lab10
{
    Slab<T> slab{};
    FixedPointIterSettings<T> iter_settings{};

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
    static auto from_file(std::istream& input)
    {
        return Lab10{
            Slab<T>::from_file(input),
            FixedPointIterSettings<T>::from_file(input)
        };
    }
};


template<std::floating_point T>
struct Outputs
{
    std::shared_ptr<Lab10<T>> inputs{};
    Flux<T> flux{};
    FixedPointIterResult<std::span<T>> result{};
    std::chrono::duration<long long, std::nano> time{}; // nanoseconds

    auto echo(std::ostream& output) const
    {
        inputs->echo(output);
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


template<class Policy, std::floating_point T>
concept ExecutionPolicy = requires(
    Policy p,
    const Slab<T>& slab,
    const FixedPointIterSettings<T>& settings,
    Flux<T>& flux
)
{
    { Policy{settings} } -> std::same_as<Policy>;
    { p.init(slab) } -> std::same_as<Flux<T>>;
    { p.run(flux) } -> std::same_as<FixedPointIterSettings<std::span<T>>>;
};


template<ExecutionPolicy Policy, std::floating_point T>
struct Lab08
{
    Info info{};
    Lab10<T> inputs{};

    [[nodiscard]]
    auto run()
    {
        const Policy policy{inputs.iter_settings};
        const auto start = std::chrono::high_resolution_clock::now();
        auto flux = policy.init(inputs.slab);
        const auto result = policy.run(flux);
        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return Outputs<T>{
            std::make_shared<Lab10<T>>(this->inputs),
            std::move(flux),
            std::move(result),
            duration
        };
    }

    [[nodiscard]]
    auto read_input_file(const std::string& filename)
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

        return T::from_file(input);
    }

};

#endif //LAB08_H
