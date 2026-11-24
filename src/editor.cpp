#include <ftxui/ftxui.hpp>

import std;
import betterman.colors;
import betterman.logger;

namespace fs = std::filesystem;
using namespace ftxui;
extern fs::path task_fp;

void Edit() {
    std::ifstream file{task_fp};
    if (Log::fileOpenError(file, task_fp.string()))
        return;

    std::stringstream str;
    str << file.rdbuf();
    std::string tasks{str.str()};
    file.close();

    bool finished{false};
    auto app = App::TerminalOutput();

    InputOption ip_opt;
    ip_opt.transform = [](InputState s) { return s.element; };

    auto placeholder = "Task file empty...";
    Component text_area;
    if (tasks.empty())
        text_area = Input(&tasks, placeholder, ip_opt);
    else
        text_area = Input(&tasks, ip_opt);
    text_area |= CatchEvent([&](Event event) {
        if (event != Event::Escape)
            return false;
        finished = true;
        app.ExitLoopClosure()();
        return true;
    });

    // clang-format off
    auto renderer = Renderer(text_area, [&] {
        return vbox({
            window(text("Task List") | color(Pallete::heading1),
                   hbox({
                        text(" "),
                        text_area->Render(),
                    }) | size(HEIGHT, LESS_THAN, 8)| color(Pallete::text)
            ) | color(Pallete::border),
            hbox({
                text(" Esc ") | bold,
                text("save & exit |") | dim,
                text(" Ctrl C ") | bold,
                text("exit") | dim,
            }) | color(Pallete::text),
        }) | size(WIDTH, EQUAL, 114);
    });
    // clang-format on

    renderer |= CatchEvent([](Event event) {
        if (event == Event::Custom) {
            Pallete::setTheme();
            return true;
        }
        return false;
    });
    app.Loop(renderer);

    if (finished) {
        std::ofstream file{task_fp};
        if(Log::fileOpenError(file, task_fp.string()))
            return;
        file << tasks;
    }
}
