#include "colors.hpp"
#include "json.hpp"
#include <filesystem>
#include <fstream>

namespace {
    nlohmann::json loadThemeConfig() {
        std::ifstream file{"themes/theme.txt"};
        std::string path_str;
        std::getline(file, path_str);
        
        std::filesystem::path path = path_str.substr(1, path_str.size() - 2);
        // std::filesystem::path path = path_str;
        std::ifstream f{path};
        return nlohmann::json::parse(f);
    }
}

// definitions of the static data members (default-constructed for now,
// the constructor below fills in the real values)
std::string  Pallete::theme_name;
ftxui::Color Pallete::text;
ftxui::Color Pallete::heading1;
ftxui::Color Pallete::border;
ftxui::Color Pallete::hover_bg;
ftxui::Color Pallete::tile_0;
ftxui::Color Pallete::tile_25;
ftxui::Color Pallete::tile_50;
ftxui::Color Pallete::tile_75;
ftxui::Color Pallete::tile_100;

void Pallete::setTheme() {
    auto config = loadThemeConfig();

    theme_name = config["name"];
    text       = rgb(config["text"][0],       config["text"][1],       config["text"][2]);
    heading1   = rgb(config["heading1"][0],   config["heading1"][1],   config["heading1"][2]);
    border     = rgb(config["border"][0],     config["border"][1],     config["border"][2]);
    hover_bg   = rgb(config["hover_bg"][0],   config["hover_bg"][1],   config["hover_bg"][2]);
    tile_0     = rgb(config["tile_0"][0],     config["tile_0"][1],     config["tile_0"][2]);
    tile_25    = rgb(config["tile_25"][0],    config["tile_25"][1],    config["tile_25"][2]);
    tile_50    = rgb(config["tile_50"][0],    config["tile_50"][1],    config["tile_50"][2]);
    tile_75    = rgb(config["tile_75"][0],    config["tile_75"][1],    config["tile_75"][2]);
    tile_100   = rgb(config["tile_100"][0],   config["tile_100"][1],   config["tile_100"][2]);
}

Pallete::Pallete(){ setTheme(); }

// the single instance — its construction is what actually invokes pallete::pallete()
Pallete Pallete::initializer;
