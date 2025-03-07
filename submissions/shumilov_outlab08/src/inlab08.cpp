#include <cstdlib>
#include <filesystem>
#include <utility>

#include <argparse/argparse.hpp>
#include <fmt/color.h>
#include <fmt/ostream.h>

#include "inlab08.h"
#include "io.h"

using real = long double;


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

        auto lab = read_input_file<inlab08<real>>(program.get<std::string>("input"));

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
