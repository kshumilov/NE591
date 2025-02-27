#include <concepts>
#include <fstream> // ifstream
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>

// 3rd-party Dependencies
#include <argparse/argparse.hpp>
#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

// Local Implementations
// LinAlg and LU
#include "methods/linalg/matrix.h"
#include "methods/linalg/utils/io.h"

// IO and Array manipulation
#include "methods/utils/io.h"


#include "project/diffusion_problem.h"
#include "project/diffusion_solver.h"


using json = nlohmann::json;


template<class T>
auto parse_input(std::istream &input, const bool as_json = false) -> IsotropicSteadyStateDiffusion2D<T> {
    if (as_json) {
        json data = json::parse(input);
        return data.template get<IsotropicSteadyStateDiffusion2D<T>>();
    }

    [[maybe_unused]] const auto flag = read_value<int>(input);
    return IsotropicSteadyStateDiffusion2D<double>::from_file(input);
}


int main(const int argc, char *argv[]) {
    const std::string title{"NE 591 Project #01"};
    const std::string author{"Kirill Shumilov"};
    const std::string date{"02/14/2025"};
    const std::string description{"Solving 2D steady state, one speed diffusion equation in a non-multiplying,\n"
                                  "isotropic scattering homogeneous medium, using LUP factorization"};


    argparse::ArgumentParser program{
            "shumilov_project01",
            "1.0",
            argparse::default_arguments::help,
    };

    program.add_description(description);

    program.add_argument("filename").help("Path to input file");

    program.add_argument("--input-json").help("Read the input file in json-format").flag();

    program.add_argument("-o", "--output").help("Path to output file");

    program.add_argument("--output-json").help("Write the output file in json-format").flag();

    program.add_argument("--quiet").help("If present suppresses output to stdout").flag();

    try {
        program.parse_args(argc, argv);
        const auto input_filename = program.get<std::string>("filename");

        std::ifstream in{input_filename};

        if (!in.is_open()) {
            throw std::runtime_error(fmt::format("Could not open '{}'", input_filename)); // Indicate an error occurred
        }

        try {
            if (not program.get<bool>("--quiet")) {
                fmt::print(
                        "================================================================================\n"
                        "{:s}\n"
                        "Author: {:s}\n"
                        "Date: {:s}\n"
                        "--------------------------------------------------------------------------------\n"
                        "{:s}\n"
                        "================================================================================\n",
                        title,
                        author,
                        date,
                        description);
            }

            const auto from_json = program.get<bool>("--input-json");
            auto problem = parse_input<double>(in, from_json);
            in.close();

            const LUPSolver solver{};
            const auto solution = solver.solve(problem);

            const auto to_json = program.get<bool>("--output-json");
            const auto output_filename = program.present<std::string>("--output");

            if (output_filename.has_value() and to_json) {
                const json j = solution;
                std::ofstream o(output_filename.value());
                o << std::setw(4) << j << std::endl;
                o.close();
            }
            else if (output_filename.has_value() and not to_json) {
                std::ofstream o(output_filename.value());
                solution.echo(o);
                o.close();
            }
            else if (not output_filename.has_value() and to_json) {
                const json j = solution;
                std::cout << std::setw(4) << j << std::endl;
            }
            else {
                solution.echo(std::cout);
            }
        }
        catch ([[maybe_unused]] const std::exception &err) {
            in.close();
            throw;
        }
    }
    catch (const std::exception &err) {
        std::cerr << "\n" << format(fmt::emphasis::bold | fg(fmt::color::red), "Error: ") << err.what() << "\n\n";
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
