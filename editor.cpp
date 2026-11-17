#include <fstream>
#include <sstream>
#include <string>

#include <ftxui/ftxui.hpp>
using namespace ftxui;

void Edit() {
    std::ifstream file{"tasks.txt"};
    std::stringstream str;
    // str << file.rdbuf();

    std::vector<std::string> task_list;
    std::string task;
    while (getline(file, task)) {
        task_list.push_back(task);
    }

    auto app = App::FullscreenAlternateScreen();

    MenuOption menu_opt;
    menu_opt.entries_option.transform = [&](EntryState s) {
        auto gutter = s.focused ? "> " : "  ";
        return hbox({text(gutter), Input(s.label)->Render()});
    };
    int selected = 0;
    auto menu = Menu(&task_list, &selected) | border;
    menu |= CatchEvent([&app](Event event) {
        if (event != Event::Escape)
            return false;
        // TODO: write to file
        app.ExitLoopClosure()();
        return true;
    });

    std::string tasks{str.str()};

    InputOption ip_opt;
    ip_opt.transform = [](InputState s) { return s.element; };
    auto text_area = Input(&tasks, ip_opt);
    text_area |= CatchEvent([&app](Event event) {
        if (event != Event::Escape)
            return false;
        // TODO: write to file
        app.ExitLoopClosure()();
        return true;
    });
    auto editor = Window({
                      .inner = text_area,
                      .title = "Task List",
                  }) |
                  flex;

    app.Loop(menu);
}
