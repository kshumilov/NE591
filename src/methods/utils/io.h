#ifndef UTILS_IO_H
#define UTILS_IO_H

#include <istream>
#include <stdexcept>
#include <optional>
#include <vector>


template<class T>
auto read_value(std::istream& in) -> std::optional<T>
{
    T result{};

    if (in >> result) {
        return std::make_optional<T>(result);
    }

    return std::nullopt;
}



template<class T>
auto read_vector(std::istream& is, const std::size_t size) -> std::vector<T>
{
    std::vector<T> values{};

    while (values.size() < size) {
        if (const auto result = read_value<T>(is); result.has_value()) {
            values.emplace_back(result.value());
        }
        else {
            throw std::runtime_error(
                std::format("Could not read vector element {:d}.", values.size())
            );
        }
    }

    return values;
}
#endif // UTILS_IO_H
