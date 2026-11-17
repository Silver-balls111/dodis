#include <fstream>
#include <sstream>
#include <string>

#include <ftxui/ftxui.hpp>
using namespace ftxui;

void Edit() {
    std::ifstream file{"tasks.txt"};
    std::stringstream str;
    str << file.rdbuf();
    std::string tasks{str.str()};
    file.close();

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
        std::ofstream file{"tasks.txt"};
        file << tasks;
        app.ExitLoopClosure()();
        return true;
    });

    // clang-format off
    auto renderer = Renderer(text_area, [&] {
        return vbox({
            window(text("Task List"),
                   hbox({
                        text(" "),
                        text_area->Render(),
                    }) | size(HEIGHT, LESS_THAN, 8)| color(Color::White)
            ) | color(Color::Green),
            hbox({
                text(" Esc ") | bold,
                text("save & exit |") | dim,
                text(" Ctrl C ") | bold,
                text("exit") | dim,
            }),
        }) | size(WIDTH, EQUAL, 114);
    });
    // clang-format on

    app.Loop(renderer);
}
