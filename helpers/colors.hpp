#pragma once
#include <ftxui/screen/color.hpp>

inline constexpr auto rgb = ftxui::Color::RGB;

namespace pallete {
static auto text = rgb(205, 214, 244);
static auto border = rgb(158, 131, 195);
static auto heading1 = rgb(205, 214, 244);
static auto hover_bg = rgb(203, 166, 247); // purpulish
static auto tile_0 = rgb(30, 30, 46);      // 128, 128, 128
static auto tile_25 = rgb(60, 52, 79);     // 69, 52, 77
static auto tile_50 = rgb(114, 95, 142);   // 138, 95, 127
static auto tile_75 = rgb(158, 131, 195);  // 195, 131, 168
static auto tile_100 = rgb(203, 166, 247); // 252, 166, 209
} // namespace pallete
