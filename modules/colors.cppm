module;
#include <nlohmann/json.hpp>
#include <ftxui/screen/color.hpp>
export module betterman.colors;

import std;
import betterman.themeFile;
import betterman.logger;

inline constexpr auto rgb = ftxui::Color::RGB;

namespace {
// clang-format off
nlohmann::json default_scheme = 
    {
        {"name",        "Default"},
        {"text",        {158, 193, 204}},
        {"heading1",    {158, 193, 204}},
        {"border",      {127, 165, 175}},
        {"hover_bg",    {127, 165, 175}},
        {"tile_0",      {31, 29, 39}},
        {"tile_25",     {63, 71, 84}},
        {"tile_50",     {98, 122, 134}},
        {"tile_75",     {127, 165, 134}},
        {"tile_100",    {156, 207, 216}},
    };
// clang-format on

nlohmann::json loadThemeConfig() {
    ThemeFile tf;
    auto path = tf.getActualThemeFilePath();
    try {
        std::ifstream f{path};
        return nlohmann::json::parse(f);
    } catch (const std::exception& e) {
        Log::log("ERROR", e.what());
        return default_scheme;
    }
}
} // namespace

export struct Pallete {
    inline static std::string  theme_name;
    inline static ftxui::Color text;
    inline static ftxui::Color heading1;
    inline static ftxui::Color border;
    inline static ftxui::Color hover_bg;
    inline static ftxui::Color tile_0;
    inline static ftxui::Color tile_25;
    inline static ftxui::Color tile_50;
    inline static ftxui::Color tile_75;
    inline static ftxui::Color tile_100;

    static void setTheme();
};

// clang-format off
void Pallete::setTheme() {
    auto config = loadThemeConfig();

    theme_name  = config["name"];
    text        = rgb(config["text"][0],     config["text"][1],     config["text"][2]);
    heading1    = rgb(config["heading1"][0], config["heading1"][1], config["heading1"][2]);
    border      = rgb(config["border"][0],   config["border"][1],   config["border"][2]);
    hover_bg    = rgb(config["hover_bg"][0], config["hover_bg"][1], config["hover_bg"][2]);
    tile_0      = rgb(config["tile_0"][0],   config["tile_0"][1],   config["tile_0"][2]);
    tile_25     = rgb(config["tile_25"][0],  config["tile_25"][1],  config["tile_25"][2]);
    tile_50     = rgb(config["tile_50"][0],  config["tile_50"][1],  config["tile_50"][2]);
    tile_75     = rgb(config["tile_75"][0],  config["tile_75"][1],  config["tile_75"][2]);
    tile_100    = rgb(config["tile_100"][0], config["tile_100"][1], config["tile_100"][2]);
}
// clang-format on
