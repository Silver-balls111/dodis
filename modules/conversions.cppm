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
}
