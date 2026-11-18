#include "conversions.hpp"

namespace chrono = std::chrono;

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
        return pallete::tile_0;
    if (opacity <= 0.25)
        return pallete::tile_25;
    if (opacity <= 0.50)
        return pallete::tile_50;
    if (opacity <= 0.75)
        return pallete::tile_75;
    return pallete::tile_100;
}
