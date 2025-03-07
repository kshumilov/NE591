#ifndef IO_H
#define IO_H

#include <string>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <concepts>

#include "fmt/color.h"


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
#endif //IO_H
