#include <cstdlib>

#include <iostream>

#include <fmt/color.h>
#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>

#include "project02.h"

using real = long double;
using json = nlohmann::json;

auto main(int argc, char* argv[]) -> int
{
    const Header header{};

    argparse::ArgumentParser program{
            "shumilov_projec02",
            "1.0",
            argparse::default_arguments::help,
    };

    program.add_argument("input").help("Path to input file.");

    program.add_argument("-o", "--output").help("Path to output file");
    program.add_argument("--output-json").help("Write the output file in json-format").flag();
    program.add_argument("--input-json").help("Path to parameter files").flag();

    try
    {
        program.parse_args(argc, argv);

        auto project = read_input_file<real>(
            program.get<std::string>("input"),
            program.get<bool>("--input-json")
        );
        const auto solution = project.run();

        const auto to_json = program.get<bool>("--output-json");

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

            if (to_json)
            {
                const json j = solution;
                output << std::setw(4) << j << std::endl;
            }
            else
            {
                header.echo(output);
                solution.echo(output);
            }

            output.close();
        }
        else {
            if (to_json)
            {
                const json j = solution;
                std::cout << std::setw(4) << j << std::endl;
            }
            else
            {
                header.echo(std::cout);
                solution.echo(std::cout);
            }
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
