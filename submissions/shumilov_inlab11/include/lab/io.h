#ifndef LAB_UTILS_IO_H
#define LAB_UTILS_IO_H

#include <string>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <concepts>

#include "fmt/core.h"


template <typename T>
concept HasFromFile = requires(std::istream& input) {
    { T::from_file(input) } -> std::same_as<T>;
};


template<HasFromFile T>
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


inline std::variant<std::reference_wrapper<std::ostream>, std::ofstream>
get_output_stream(const std::optional<std::string>& filename) {
    if (!filename) {
        // If no filename provided, return standard output stream
        return std::ref(std::cout);
    }

    // Try to open the file
    std::ofstream file_stream(filename.value(), std::ios::out);

    // Check if file was successfully opened
    if (!file_stream.is_open()) {
        // If file cannot be opened, throw an exception
        throw std::runtime_error(
            fmt::format("Could not open: '{}'", filename.value())
        );
    }

    // Return the file stream
    return file_stream;
}

#endif // LAB_UTILS_IO_H
