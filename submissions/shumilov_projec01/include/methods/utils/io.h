#ifndef UTILS_IO_H
#define UTILS_IO_H

#include <istream>
#include <stdexcept>
#include <optional>
#include <typeinfo>
#include <vector>
#include <string_view>

#include <fmt/core.h>

using namespace std::literals;


template<class T>
auto read_value(std::istream& in) -> std::optional<T>
{
    T result{};

    if (in >> result) {
        return std::make_optional<T>(result);
    }

    return std::nullopt;
}


template<class ReturnType, class RawType = ReturnType>
auto read_and_validate(
    std::istream& in,
    std::invocable<RawType> auto validate,
    std::invocable<> auto on_read_error
) -> ReturnType
{
    if (const auto raw = read_value<RawType>(in); raw.has_value()) {
        return validate(raw.value());
    }
    throw on_read_error();
}


template<class ReturnType>
auto read_positive_value(std::istream& in, std::string_view name = "value"sv)
{
    auto is_positive = [](const auto x) -> bool {
        return x > 0;
    };

    return read_and_validate<ReturnType>(
        in,
        [&](const auto v) {
            if (is_positive(v))
                return v;

            throw std::runtime_error(fmt::format(
                "Invalid `{}`, must be a positive {}: {}", name, v, typeid(ReturnType).name()
            ));
        },
        [&]() {
            return std::runtime_error(fmt::format(
                "Could not read `{}`", name
            ));
        }
    );
}


template<class ReturnType>
auto read_nonnegative_value(std::istream& in, std::string_view name = "value"sv)
{
    auto is_nonnegative = [](const auto x) -> bool {
        return x >= 0;
    };

    return read_and_validate<ReturnType>(
        in,
        [&](const auto v) {
            if (is_nonnegative(v))
                return v;

            throw std::runtime_error(fmt::format(
                "Invalid `{}`, must be a positive {}: {}", name, v, typeid(ReturnType).name()
            ));
        },
        [&]() {
            return std::runtime_error(fmt::format(
                "Could not read `{}`", name
            ));
        }
    );
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
