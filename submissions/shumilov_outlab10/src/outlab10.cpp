#include <cstdlib>
#include <filesystem>
#include <iostream>

#include <argparse/argparse.hpp>

#include <fmt/color.h>
#include <fmt/ostream.h>

#include "lab/io.h"

#include "Lab10.h"


using real = long double;


int main(int argc, char *argv[])
{
    argparse::ArgumentParser program{
        "shumilov_outlab10",
        "1.0",
        argparse::default_arguments::help,
    };

    program.add_argument("input").help("Path to input file.");
    program.add_argument("-o", "--output").help("Path to output file");

    try
    {
        program.parse_args(argc, argv);

        const auto lab = read_input_file<Lab10<real>>(
            program.get<std::string>("input")
        );

        const auto result = lab.run();

        auto output = get_output_stream(
            program.present<std::string>("--output")
        );

        std::visit(
            [&](auto& stream)
            {
                fmt::println(
                    stream,
                    "{}",
                    lab
                );

                Lab10<real>::print_result(stream, result);
            },
            output
        );
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
