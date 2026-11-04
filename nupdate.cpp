#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include <ftxui/ftxui.hpp>

void Update() {
    using namespace ftxui;

    std::ifstream file{"tasks.txt"};

    std::vector<std::string> tasks;

    while (file) {
        std::string task;
        getline(file, task);
        tasks.push_back("   " + task);
    }
    file.close();

    std::string_view check{"  "};
    std::string_view cross{"  "};
    std::string_view empty{"   "};

    std::vector<bool> done(tasks.size());
    std::vector<bool> not_done(tasks.size());
    std::vector<std::string_view> icon(tasks.size());
    for (auto x : icon)
        x = empty;

    MenuOption opt;
    opt.entries_option.transform = [&](EntryState state) {
        Element gutter = text(icon[state.index]);
        Element label = text(state.label);
        if (done[state.index]) {
            gutter |= color(Color::Green);
            label = label | strikethrough | color(Color::Green) | dim;
        }
        if (not_done[state.index]) {
            gutter |= color(Color::Red);
            label = label | strikethrough | color(Color::Red);
        }
        Element combined = hbox({gutter, label});
        if (state.focused) {
            // TODO: choose better hover color
            gutter |= inverted;
            label |= inverted;
        }
        return hbox({gutter, label});
    };

    // Look we need a extra copy that we can use to get the orginial label or
    // else the ternary would just eat 3 chars from label until seg fault.
    const std::vector<std::string> task_copy = tasks;
    int selected = 0;

    auto task_list = Menu(&tasks, &selected, opt);

    task_list |= CatchEvent([&](Event event) {
        if (event == Event::Return) {
            not_done[selected] = false;
            done[selected] = !done[selected];
            tasks[selected] = done[selected] ? task_copy[selected].substr(3)
                                             : tasks[selected];
            icon[selected] = done[selected] ? check : empty;
            return true;
        }

        if (event == Event::Character('x')) {
            done[selected] = false;
            not_done[selected] = !not_done[selected];
            tasks[selected] = not_done[selected] ? task_copy[selected].substr(3)
                                                 : tasks[selected];
            icon[selected] = not_done[selected] ? cross : empty;
            return true;
        }
        return false;
    });

    auto renderer = Renderer(task_list, [&] {
        return task_list->Render() | vscroll_indicator | yframe |
               size(HEIGHT, LESS_THAN, 8) | border;
    });

    auto screen = App::TerminalOutput();
    screen.Loop(renderer);
}
