#include <cstdlib>

#include <fmt/format.h>
#include <fmt/core.h>

#include <methods/optimize.h>
#include <methods/fixed_point.h>


// The struct we want to format
struct MyStruct {
    float value{1234.9};
    std::string name{"Kirill"};
};

// Custom formatter for MyStruct
template <>
struct fmt::formatter<MyStruct> : fmt::formatter<std::string> {
private:
    // Store format specifiers for float and string
    fmt::formatter<float> real_fmt{};
    fmt::formatter<std::string> label_fmt{};

    fmt::parse_context<> default_real_fmt{".6g"};
    fmt::parse_context<> default_label_fmt{".<40s"};

public:
    // Parse method to handle format specifiers
    constexpr auto parse(fmt::format_parse_context& ctx) {
        auto reached_end = [&] (const auto& pos) -> bool
        {
            return pos == ctx.end() or *pos == '}';
        };

        auto it = ctx.begin();

        // Check that everything is defaulted
        if (reached_end(it))
        {
            real_fmt.parse(default_real_fmt);
            label_fmt.parse(default_label_fmt);
            return it;
        }

        // Parse String format
        if (*it != ':')
            it = label_fmt.parse(ctx);

        if (reached_end(it))
        {
            real_fmt.parse(default_real_fmt);
            return it;
        }

        // Find the ':' divider and move past it
        if (*it == ':') {
            ++it;
        }

        // Parse string specification
        ctx.advance_to(it);
        return real_fmt.parse(ctx);
    }

    // Format method to format the struct
    auto format(const MyStruct& obj, fmt::format_context& ctx) const {
        auto out = label_fmt.format(obj.name, ctx);
        out = fmt::format_to(out, ": ");
        ctx.advance_to(out);
        return real_fmt.format(obj.value, ctx);
    }
};


int main()
{
    return EXIT_SUCCESS;
}