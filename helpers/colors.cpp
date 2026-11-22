#include "colors.hpp"
#include "globals.hpp"
#include "json.hpp"
#include <fstream>

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
    auto value = tf.readPsuedoFile();
    try {
        if (!value.has_value()) {
            throw;
        }
        std::ifstream f{value.value()};
        return nlohmann::json::parse(f);
    } catch (...) {
        // TODO: write to log file the error.
        return default_scheme;
    }
}
} // namespace

// definitions of the static data members
// (default-constructed for now, the
// constructor below fills in the real
// values)
std::string Pallete::theme_name;
ftxui::Color Pallete::text;
ftxui::Color Pallete::heading1;
ftxui::Color Pallete::border;
ftxui::Color Pallete::hover_bg;
ftxui::Color Pallete::tile_0;
ftxui::Color Pallete::tile_25;
ftxui::Color Pallete::tile_50;
ftxui::Color Pallete::tile_75;
ftxui::Color Pallete::tile_100;

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

Pallete::Pallete() { setTheme(); }

// the single instance — its construction
// is what actually invokes
// pallete::pallete()
Pallete Pallete::initializer;
