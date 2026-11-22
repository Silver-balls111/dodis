#include "colors.hpp"
#include "globals.hpp"
#include <filesystem>
#include <fstream>
#include <ftxui/ftxui.hpp>
#include <iostream>
#include <map>
#include <mutex>

namespace fs = std::filesystem;

// TODO: Add a filewatcher for file path in theme.txt when that path file
// gets modified call setTheme.

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
            ThemeFile tf;
            std::string path = themes[theme_keys[current]];
            auto status = tf.writePsuedoFile(path);
            if (!status.has_value()) {
                std::cerr << status.error();
            }
            return true;
        }
        return false;
    });

    auto menu_renderer = Renderer(theme_menu, [&] {
        return theme_menu->Render() | vscroll_indicator | yframe |
               size(HEIGHT, LESS_THAN, 8) | borderStyled(Pallete::border);
    });

    auto renderer = Renderer(menu_renderer, [&] {
        return vbox({
                   menu_renderer->Render(),
                   hbox({
                       text(" Enter ") | bold,
                       text("select theme | ") | dim,
                       text("Esc/q ") | bold,
                       text("exit") | dim,
                   }) | color(Pallete::text),
               }) |
               size(WIDTH, LESS_THAN, 114);
    });

    renderer |= CatchEvent([](Event event) {
        if (event == Event::Custom) {
            Pallete::setTheme();
            return true;
        }
        return false;
    });

    screen.Loop(renderer);
}
