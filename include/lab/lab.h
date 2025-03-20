#ifndef LAB_H
#define LAB_H

#include <string>

#include <fmt/core.h>

#include "lab/config.h"


struct Info
{
    std::string title{ "NE 591 Inlab #08" };
    std::string author{ "Kirill Shumilov" };
    std::string date{ "02/28/2025" };
    std::string description{ "Solving Source Iteration Equations" };


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

#endif //LAB_H
