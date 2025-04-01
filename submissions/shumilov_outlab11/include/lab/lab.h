#ifndef LAB_INFO_H
#define LAB_INFO_H

#include <string>

#include <fmt/format.h>

#include "lab/config.h"


struct Info
{
    std::string title{};
    std::string author{};
    std::string date{};
    std::string description{};

    [[nodiscard]] constexpr auto to_string() const -> std::string
    {
        return fmt::format(
            "{0:=^{1}}\n"
            "{2:s}\n"
            "Author: {3:s}\n"
            "Date: {4:s}\n"
            "{0:-^{1}}\n"
            "{5:s}\n"
            "{0:=^{1}}\n",
            "", COLUMN_WIDTH,
            title,
            author,
            date,
            description
        );
    }


    void echo(std::ostream& out) const { out << this->to_string(); }
};

template<>
struct fmt::formatter<Info>
{
    [[nodiscard]]
    constexpr auto parse(fmt::parse_context<>& ctx)
    {
        return ctx.begin();
    }


    [[nodiscard]]
    constexpr auto format(const Info& info, fmt::format_context& ctx) const
    {
        using namespace fmt::literals;

        return fmt::format_to(
            ctx.out(),
            "{0:=^{1}}\n"
            "{title:s}\n"
            "Author: {author:s}\n"
            "Date: {date:s}\n"
            "{0:-^{1}}\n"
            "{description:s}\n"
            "{0:=^{1}}\n",
            "", COLUMN_WIDTH,
            "title"_a=info.title,
            "author"_a=info.author,
            "date"_a=info.date,
            "description"_a=info.description
        );
    }
};

#endif // LAB_INFO_H
