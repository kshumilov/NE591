#include <filesystem>
#include <fstream>
#include <ostream>

#include <fmt/color.h>
#include <fmt/ostream.h>

#include <argparse/argparse.hpp>

#include "lab06.h"


template<std::floating_point DType>
[[nodiscard]]
auto read_input_file(const std::string &filename) -> Outlab6<DType> {
    const auto input_filepath = std::filesystem::path{filename};

    if (input_filepath.empty()) {
        throw std::runtime_error(
                fmt::format("Input file does not exist: {}", input_filepath.string())); // Indicate an error occurred
    }

    std::ifstream input{input_filepath};

    if (!input.is_open()) {
        throw std::runtime_error(
                fmt::format("Could not open input file: {}", input_filepath.string())); // Indicate an error occurred
    }

    try {
        const auto lab = Outlab6<DType>::from_file(input);
        input.close();
        return lab;
    }
    catch (const std::exception &) {
        input.close();
        throw;
    }
}


using real = double;

int main(int argc, char **argv) {
    const Header header{};

    argparse::ArgumentParser program{
            "shumilov_outlab6",
            "1.0",
            argparse::default_arguments::help,
    };

    program.add_argument("input").help("Path to input file");

    program.add_argument("-o", "--output").help("Path to output file");

    try {
        program.parse_args(argc, argv);
        const auto lab = read_input_file<real>(program.get<std::string>("input"));

        const auto result = lab.run();

        if (const auto output_filename = program.present<std::string>("--output"); output_filename.has_value()) {
            std::ofstream output{output_filename.value()};
            if (!output.is_open()) {
                throw std::runtime_error(
                        fmt::format("Could not open: '{}'", output_filename.value())); // Indicate an error occurred
            }
            header.echo(output);
            result.echo(output);
        }
        else {
            header.echo(std::cout);
            result.echo(std::cout);
        }
    }
    catch (const std::exception &err) {
        std::cerr << "\n" << format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ") << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
