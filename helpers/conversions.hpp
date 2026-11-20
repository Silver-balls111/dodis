#pragma once

#include "ftxui/screen/color.hpp"
#include <chrono>
#include <string>

std::chrono::year stringToYear(const std::string &s_year);
std::string yearToString(const std::chrono::year &year);
std::string getSuffixedDate(const unsigned int day);
ftxui::Color opacityToColor(const double opacity);
