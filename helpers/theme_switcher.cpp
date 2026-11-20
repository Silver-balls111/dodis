#include "colors.hpp"
#include <filesystem>
#include <fstream>
#include <ftxui/ftxui.hpp>
#include <map>

namespace fs = std::filesystem;

void themeSwitcher() {
    using namespace ftxui;
    std::map<std::string, fs::path> themes;

    for (const auto &entry : fs::recursive_directory_iterator("themes")) {
        if (entry.is_regular_file() &&
            entry.path().extension() == fs::path(".json")) {
            themes[entry.path().stem()] = entry.path();
        }
    }
    std::vector<std::string> theme_keys;
    theme_keys.reserve(themes.size());
    int idx{0};
    int selected;
    for (const auto &[name, path] : themes) {
        theme_keys.emplace_back(name);
        if (name == Pallete::theme_name) {
            selected = idx;
        }
        ++idx;
    }

    int current = selected;
    fs::path choosen_theme = themes[Pallete::theme_name];

    auto screen = App::TerminalOutput();

    MenuOption opt;
    opt.entries_option.transform = [&](EntryState state) {
        if (state.active)
            choosen_theme = themes[state.label];
        Color fg;
        fg = state.focused ? Pallete::hover_bg : Pallete::text;
        auto gutter = state.index == current ? text(" ◉ ") : text(" ○ ");
        auto item = hbox({gutter, text(state.label)}) | color(fg);
        if (state.focused)
            item |= inverted;
        return item;
    };

    auto theme_menu = Menu(&theme_keys, &selected, opt);
    theme_menu |= CatchEvent([&](Event event) {
        if (event == Event::q || event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Return) {
            current = selected;
            std::ofstream file {"themes/theme.txt"};
            file << themes[theme_keys[current]];
            file.flush();
            Pallete::setTheme();
            return true;
        }
        return false;
    });

    auto menu_renderer = Renderer(theme_menu, [&] {
        return theme_menu->Render() | vscroll_indicator | yframe |
               size(HEIGHT, LESS_THAN, 8) | borderStyled(Pallete::border);
    });

    auto tooltip = hbox({
                       text(" Enter ") | bold,
                       text("select theme | ") | dim,
                       text("Esc/q ") | bold,
                       text("exit") | dim,
                   }) |
                   color(Pallete::text);

    auto renderer = Renderer(menu_renderer, [&] {
        return vbox({menu_renderer->Render(), tooltip}) |
               size(WIDTH, LESS_THAN, 114);
    });
    screen.Loop(renderer);
}
