#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <ftxui/ftxui.hpp>

namespace chrono = std::chrono;

void finishUP(float final_opacity) {
    using namespace std;
    const auto today = chrono::floor<chrono::days>(chrono::system_clock::now());
    const string todays_date = format("{:%Y/%m/%d}", today);
    const string current_year = format("{:%Y}", today);
    std::filesystem::path contribution_file_path{
        std::format("contributions/{}.txt", current_year)};

    /*
    if we had just appended to the end of the file, when the program is run
    multiple time the same day there would be duplicate entry,
    thus we rewrite the file each time, this also implies if the system date
    is changed by the user to old dates then future entries will be removed.
    */
    ifstream read_contribution{contribution_file_path};
    vector<string> dates;
    vector<float> opacitys;
    stringstream buffer;
    if (read_contribution) {
        // makes buffer a pointer to the contribution file buffer
        // so we don't have to worry about whitespce while reading
        buffer << read_contribution.rdbuf();
        while (buffer) {
            string date;
            float opac;
            buffer >> date >> opac;
            if (!buffer || (date == todays_date)) {
                break;
            }
            dates.push_back(date);
            opacitys.push_back(opac);
        }
    }
    read_contribution.close();

    dates.push_back(todays_date);
    opacitys.push_back(final_opacity);

    ofstream contribution_file{contribution_file_path};
    if (!contribution_file)
        throw runtime_error("Couldn't open contributions file.");

    for (size_t i = 0; i < dates.size(); ++i) {
        contribution_file << format("{}\t{:.3}\n", dates[i], opacitys[i]);
    }

    return;
}

void Update() {
    using namespace ftxui;

    auto screen = App::TerminalOutput();
    std::ifstream file{"tasks.txt"};

    std::vector<std::string> tasks;

    std::string task;
    while (getline(file, task)) {
        tasks.push_back("   " + task);
    }
    file.close();

    std::string_view check{"  "};
    std::string_view cross{"  "};
    std::string_view empty{"   "};

    const int vec_sizes = tasks.size();
    std::vector<float> completion(vec_sizes);
    std::vector<bool> done(vec_sizes);
    std::vector<bool> not_done(vec_sizes);
    std::vector<std::string_view> icon(vec_sizes);
    for (auto x : icon)
        x = empty;

    MenuOption opt;
    opt.entries_option.transform = [&](EntryState state) {
        Element gutter = text(icon[state.index]);
        Element label = text(state.label);
        if (done[state.index]) {
            gutter |= color(Color::RGB(166, 227, 161)) | dim;
            label = label | strikethrough | color(Color::RGB(166, 227, 161)) | dim;
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
    // bool show_modal{false};
    int depth{0};
    std::string input_text;

    std::string placehol = "0-100";
    InputOption ip_opt;
    ip_opt.transform = [](InputState s) {
        if (s.is_placeholder)
            s.element |= dim;
        return hbox({text(" ") | color(Color::Green) | bold,
                     s.element}); // chevron right - f054
    };
    auto input = Input(&input_text, &placehol, ip_opt);
    input |= CatchEvent([&](Event event) {
        if (event == Event::Escape){
            input_text.clear();
            depth = 0;
            return true;
        }
        if (event != Event::Return)
            return false;
        if (input_text.empty())
            return true;
        try {
            float v{0.0};
            v = std::stof(input_text);
            if (v < 0){
                v = 0;
                done[selected] = 1;
            }
            else if (v > 100){
                v = 1;
                not_done[selected] = 1;
            }
            else
                v = v / 100;
            completion[selected] = v;
            input_text.clear();
            depth = 0;
        } catch (std::invalid_argument) {
            input_text.clear();
            placehol = "Invalid value";
        }
        return true;
    });

    Component depth_1 = Window({
        .inner = input,
        .title = "Percentage",
        .width = 20,
        .height = 3,
    });

    auto task_list = Menu(&tasks, &selected, opt);

    task_list |= CatchEvent([&](Event event) {
        if (event == Event::Return) {
            not_done[selected] = false;
            done[selected] = !done[selected];
            tasks[selected] = done[selected] ? task_copy[selected].substr(3)
                                             : tasks[selected];
            icon[selected] = done[selected] ? check : empty;
            completion[selected] = 1;
            return true;
        }

        if (event == Event::x) {
            done[selected] = false;
            not_done[selected] = !not_done[selected];
            tasks[selected] = not_done[selected] ? task_copy[selected].substr(3)
                                                 : tasks[selected];
            icon[selected] = not_done[selected] ? cross : empty;
            completion[selected] = 0;
            return true;
        }

        if (event == Event::p) {
            done[selected] = false;
            not_done[selected] = false;
            depth = 1;
        }

        if (event == Event::Escape || event == Event::q) {
            float aggregate_compl{0.0};
            for (float value : completion)
                aggregate_compl += value;
            finishUP(aggregate_compl / tasks.size());
            screen.ExitLoopClosure()();
        }
        return false;
    });

    // task_list |= Modal(modalC, &show_modal);

    auto task_renderer = Renderer(task_list, [&] {
        return task_list->Render() | vscroll_indicator | yframe |
               size(HEIGHT, LESS_THAN, 8) | border;
    });

    auto tooltip = hbox({
        text("Enter ") | bold,
        text("mark done | ") | dim,
        text("x ") | bold,
        text("mark not done | ") | dim,
        text("p ") | bold,
        text("percentage | ") | dim,
        text("Esc/q ") | bold,
        text("finish") | dim,
    });

    auto depth_0_renderer = Renderer(task_renderer, [&] {
        return vbox({task_renderer->Render(), tooltip});
    });

    auto whole = Container::Tab(
        {
            depth_0_renderer,
            depth_1,
        },
        &depth);

    auto main_renderer = Renderer(whole, [&] {
        Element document = depth_0_renderer->Render();

        if (depth == 1) {
            document = dbox({
                document,
                depth_1->Render() | clear_under | center,
            });
        }
        return document;
    });

    screen.Loop(main_renderer);
}
