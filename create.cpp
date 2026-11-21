#include <fstream>
#include <iostream>
#include <string>

#include "ftxui/component/app.hpp"       // for App
#include "ftxui/component/component.hpp" // for Input, Renderer, CatchEvent
#include "ftxui/component/event.hpp"     // for Event
#include "ftxui/dom/elements.hpp"        // for text, paragraph, vbox

#include "helpers/colors.hpp"

void Create() {
    using namespace ftxui;

    std::ofstream task_file{"tasks.txt"};
    std::ofstream compl_file{"completion.txt"};
    if (!task_file) {
        std::cerr << "Error: Opening \"tasks.txt\" file.";
        return;
    }
    if (!compl_file) {
        std::cerr << "Error: Opening \"completion.txt\" file.";
        return;
    }

    auto app = App::TerminalOutput();

    std::string input_text;
    InputOption opt;
    opt.transform = [](InputState s) {
        if (s.is_placeholder)
            s.element |= dim;
        return hbox({text(" ") | color(Pallete::border) | bold,
                     s.element | color(Pallete::text)}); // chevron right - f054
    };
    auto input = Input(&input_text, "add a task…", opt);

    bool empty_list{true};
    input |= CatchEvent([&](Event event) {
        if (event == Event::Escape)
            app.Exit();
        if (event != Event::Return) {
            return false;
        }
        if (input_text.empty()) {
            return true;
        }
        empty_list = false;
        app.PrintAbove(hbox({
            text("    │ ") | color(Pallete::border) | dim,
            text("• ") | color(Pallete::border), 
            paragraph(input_text) | color(Pallete::text)
        }));

        task_file << input_text << '\n';
        compl_file << -1 << '\n';
        input_text.clear();
        return true;
    });

    auto empty_list_msg =
        Renderer([] { return text("    No task added yet...") | dim; });
    auto conditional_empty_msg = empty_list_msg | Maybe(&empty_list);

    auto hints = hbox({
        text("Enter ") | bold,
        text("add | ") | dim,
        text("Esc ") | bold,
        text("finish") | dim,
    }) | color(Pallete::text);
    auto renderer = Renderer(input, [&] {
        return vbox({
                   conditional_empty_msg->Render(),
                   separatorHeavy() | color(Pallete::border),
                   input->Render(),
                   hints,
               }) |
               size(WIDTH, LESS_THAN, 114);
    });

    renderer |= CatchEvent([](Event event){
        if (event == Event::Custom){
            Pallete::setTheme();
            return true;
        }
        return false;
    });

    app.PrintAbove(
        vbox({
            text("To Do List") | bold | hcenter | color(Pallete::heading1),
            separator() | color(Pallete::border),
        }) |
        size(WIDTH, LESS_THAN, 114));
    app.RequestAnimationFrame();

    app.Loop(renderer);
}
