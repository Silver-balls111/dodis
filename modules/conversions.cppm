module;
#include "ftxui/screen/color.hpp"
export module betterman.conversions;

import std;
import betterman.colors;

namespace chrono = std::chrono;

export {
    chrono::year stringToYear(const std::string &s_year) {
        return chrono::year{std::stoi(s_year)};
    }

    std::string yearToString(const chrono::year &year) {
        return std::to_string(static_cast<int>(year));
    }

    std::string getSuffixedDate(const unsigned int day) {
        std::string suffix{};
        switch (day) {
        case 1:
        case 21:
        case 31:
            suffix = "st";
            break;
        case 2:
        case 22:
            suffix = "nd";
            break;
        case 3:
        case 23:
            suffix = "rd";
            break;
        default:
            suffix = "th";
        }
        return std::to_string(day) + suffix;
    }

    ftxui::Color opacityToColor(const double opacity) {
        if (opacity == 0)
            return Pallete::tile_0;
        if (opacity <= 0.25)
            return Pallete::tile_25;
        if (opacity <= 0.50)
            return Pallete::tile_50;
        if (opacity <= 0.75)
            return Pallete::tile_75;
        return Pallete::tile_100;
    }

    namespace Glyph {
    // external linkage- can be accessed across files
    // interanl linkag - only available in this file.
    // variable in namespace external linkage by default.
    // const/constexpr in namespace internal linkage.
    // so we need to inline it to make external link again.
    inline constexpr std::string_view empty{"   "};
    inline constexpr std::string_view check{"  "};
    inline constexpr std::string_view cross{"  "};
    inline constexpr std::string_view below12{"󰝦  "};
    inline constexpr std::string_view below25{"󰪞  "};
    inline constexpr std::string_view below37{"󰪟  "};
    inline constexpr std::string_view below50{"󰪠  "};
    inline constexpr std::string_view below62{"󰪡  "};
    inline constexpr std::string_view below75{"󰪢  "};
    inline constexpr std::string_view below87{"󰪣  "};
    inline constexpr std::string_view below100{"󰪤  "};
    }; // namespace Glyph

    std::string_view opacityToGlyph(float op) {
        using namespace Glyph;
        if (op == -1)
            return empty;
        else if (op == 0)
            return cross;
        else if (op < 0.12)
            return below12;
        else if (op < 0.25)
            return below25;
        else if (op < 0.37)
            return below37;
        else if (op < 0.50)
            return below50;
        else if (op < 0.62)
            return below62;
        else if (op < 0.75)
            return below75;
        else if (op < 0.87)
            return below87;
        else if (op < 1)
            return below100;
        else
            return check;
    }
}
