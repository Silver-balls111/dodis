#pragma once
#include <string>
#include <ftxui/screen/color.hpp>

inline constexpr auto rgb = ftxui::Color::RGB;

struct Pallete {
    static std::string  theme_name;
    static ftxui::Color text;
    static ftxui::Color heading1;
    static ftxui::Color border;
    static ftxui::Color hover_bg;
    static ftxui::Color tile_0;
    static ftxui::Color tile_25;
    static ftxui::Color tile_50;
    static ftxui::Color tile_75;
    static ftxui::Color tile_100;

    Pallete();           // loads theme from disk into the statics above
    static void setTheme();

private:
    static Pallete initializer;   // the ONE instance that forces the ctor to run
};
