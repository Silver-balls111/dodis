#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <ftxui/ftxui.hpp>

import std;
import betterman.colors;
import betterman.conversions;
import betterman.calender;
import betterman.logger;

namespace fs = std::filesystem;
namespace chrono = std::chrono;

extern fs::path base;
extern fs::path contributions_dir;
extern fs::path task_fp;
extern fs::path completion_fp;

ftxui::Color lerpColor(double t) { // t in [0,1]
    auto lerp = [](int a, int b, double t) {
        return static_cast<uint8_t>(a + (b - a) * t);
    };
    return ftxui::Color::RGB(lerp(255, 166, t), lerp(180, 227, t),
                             lerp(171, 161, t));
}

void finishUP(float final_opacity, std::vector<double> compl_level,
              std::vector<int> indices) {
    using namespace std;
    const string today{getZonedDate()};
    const string current_year = std::format("{:%Y}", getZonedDate<false>());

    if (!fs::exists(contributions_dir)) {
        fs::create_directory(contributions_dir);
        std::string msg = "Created directory: " + contributions_dir.string();
        Log::log("INFO", msg);
    }
    /*
    if we had just appended to the end of the file, when the program is run
    multiple time the same day there would be duplicate entry,
    thus we rewrite the file each time, this also implies if the system date
    is changed by the user to old dates then future entries will be removed.
    */
    std::string file_name = current_year + ".txt";
    fs::path contribution_file_path = contributions_dir / file_name;
    vector<string> dates;
    vector<float> opacitys;

    // if it dosen't exist it's prolly first run.
    if (fs::exists(contribution_file_path)) {
        ifstream read_contribution{contribution_file_path};
        if (Log::fileOpenError(read_contribution,
                               contribution_file_path.string()))
            exit(1);

        stringstream buffer;
        if (read_contribution) {
            // makes buffer a pointer to the contribution file buffer
            // so we don't have to worry about whitespce while reading
            buffer << read_contribution.rdbuf();
            while (buffer) {
                string date;
                float opac;
                buffer >> date >> opac;
                if (!buffer || (date == today)) {
                    break;
                }
                dates.push_back(date);
                opacitys.push_back(opac);
            }
        }
        read_contribution.close();
    }

    dates.push_back(today);
    opacitys.push_back(final_opacity);

    ofstream contribution_file{contribution_file_path};
    if (Log::fileOpenError(contribution_file, contribution_file_path.string()))
        exit(1);

    for (size_t i = 0; i < dates.size(); ++i) {
        contribution_file << format("{}\t{:.3}\n", dates[i], opacitys[i]);
    }

    // std::ranges::sort(indices);
    std::vector<double> sorted_comp(compl_level.size());
    // sorted_comp.reserve(compl_level.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        sorted_comp[indices[i]] = compl_level[i];
    }

    std::ofstream compl_file{completion_fp};
    if (Log::fileOpenError(compl_file, completion_fp.string()))
        exit(1);

    for (double x : sorted_comp) {
        compl_file << x << '\n';
    }
    return;
}

std::vector<int> startUP(std::vector<double> &compl_level) {
    std::string today{getZonedDate()};

    auto lwt{fs::last_write_time(completion_fp)};
    auto s_lwt = chrono::clock_cast<chrono::system_clock>(lwt);
    chrono::zoned_time z_lwt{chrono::current_zone(), s_lwt};
    std::string last_modified = format("{:%Y/%m/%d}", z_lwt);

    std::ifstream compl_file{completion_fp};
    if (Log::fileOpenError(compl_file, completion_fp.string()))
        exit(1);

    std::stringstream abc;
    abc << compl_file.rdbuf();
    double lvl;
    if (today != last_modified) {
        while (abc >> lvl)
            compl_level.push_back(-1);
    } else {
        while (abc >> lvl) {
            compl_level.push_back(lvl);
        }
    }
    std::vector<int> indices(compl_level.size());
    std::iota(indices.begin(), indices.end(), 0);

    if (today == last_modified) {
        std::ranges::stable_sort(indices, [&](int A, int B) -> bool {
            return compl_level[A] < compl_level[B];
        });
        std::ranges::stable_sort(compl_level);
    }

    return std::move(indices);
}

void Update() {
    using namespace ftxui;

    // NOTE: not sure whether to use altscr or just clear in main() using ansi
    // escp

    std::vector<double> compl_levels;
    std::vector<int> indices = startUP(compl_levels);
    const int tasks_len = compl_levels.size();

    std::ifstream task_file{task_fp};
    if (Log::fileOpenError(task_file, task_fp.string()))
        exit(1);

    std::string task;
    std::vector<std::string> unsorted_tasks;
    unsorted_tasks.reserve(tasks_len);
    std::vector<std::string_view> icon;
    icon.reserve(tasks_len);
    for (int i = 0; getline(task_file, task); ++i) {
        unsorted_tasks.emplace_back(task);
        icon.emplace_back(opacityToGlyph(compl_levels[i]));
    }
    std::vector<std::string> tasks;
    tasks.reserve(tasks_len);
    for (int idx : indices)
        tasks.emplace_back(unsorted_tasks[idx]);
    tasks.reserve(tasks_len);

    task_file.close();

    bool finished{false};
    auto screen = App::TerminalOutput();

    MenuOption opt;
    opt.entries_option.transform = [&](EntryState state) {
        const double level = compl_levels[state.index];
        const bool resolved = (level == 0.0 || level == 1.0);
        const bool untouched = (level < 0);
        const Color grey_blue = Pallete::hover_bg;

        Color fg;
        if (untouched)
            fg = state.focused ? grey_blue : Pallete::text;
        else
            fg = lerpColor(level);

        Element l_padding = text(" ");
        Element gutter = paragraph(icon[state.index]) | color(fg);
        Element label = paragraph(state.label) | color(fg);
        if (resolved)
            label |= strikethrough;

        auto hi = hbox({l_padding, gutter, label});
        if (resolved)
            hi |= dim;
        if (state.focused)
            hi = hi | color(fg) | inverted;
        return hi;
    };

    int selected = 0;
    int depth{0};
    std::string input_text;

    std::string placeholder = "0-100";
    InputOption ip_opt;
    ip_opt.transform = [](InputState s) {
        if (s.is_placeholder)
            s.element |= color(Pallete::text) | dim;
        return hbox({text(" ") | color(Pallete::border) | bold,
                     s.element | color(Pallete::text)}); // chevron right - f054
    };
    auto input = Input(&input_text, &placeholder, ip_opt);
    input |= CatchEvent([&](Event event) {
        if (event == Event::Escape) {
            input_text.clear();
            depth = 0;
            return true;
        }
        if (event != Event::Return)
            return false;
        if (input_text.empty())
            return true;
        try {
            double v{0.0};
            // NOTE: stof will work as long as text starts with a float value
            // so no need to check for % or anything else.
            if (input_text.ends_with('%'))
                input_text = input_text.substr(0, input_text.size() - 1);
            v = std::stof(input_text);
            if (v < 0)
                v = 0;
            else if (v > 100)
                v = 1;
            else
                v = v / 100;
            compl_levels[selected] = v;
            icon[selected] = opacityToGlyph(v);
            input_text.clear();
            depth = 0;
        } catch (std::invalid_argument) {
            input_text.clear();
            placeholder = "Invalid value";
        }
        return true;
    });

    auto task_menu = Menu(&tasks, &selected, opt);

    task_menu |= CatchEvent([&](Event event) {
        if (event == Event::Return) {
            compl_levels[selected] = compl_levels[selected] == 1 ? -1 : 1;
            icon[selected] =
                compl_levels[selected] == 1 ? Glyph::check : Glyph::empty;
            return true;
        }

        if (event == Event::x) {
            compl_levels[selected] = compl_levels[selected] == 0 ? -1 : 0;
            icon[selected] =
                compl_levels[selected] == 0 ? Glyph::cross : Glyph::empty;
            return true;
        }

        if (event == Event::p) {
            depth = 1;
            return true;
        }

        if (event == Event::Escape || event == Event::q) {
            screen.ExitLoopClosure()();
            finished = true;
        }
        return false;
    });

    auto task_renderer = Renderer(task_menu, [&] {
        return task_menu->Render() | vscroll_indicator | yframe |
               size(HEIGHT, LESS_THAN, 8) | borderStyled(Pallete::border);
    });

    // clang-format off
    auto depth_0_renderer = Renderer(task_renderer, [&] {
        return vbox({
            task_renderer->Render(), 
            hbox({
                 text(" Enter ") | bold,
                 text("mark done | ") | dim,
                 text("x ") | bold,
                 text("mark not done | ") | dim,
                 text("p ") | bold,
                 text("percentage | ") | dim,
                 text("Esc/q ") | bold,
                 text("finish") | dim,
             }) | color(Pallete::text),
        });
    });
    // clang-format on

    auto depth_1_renderer = Renderer(input, [&] {
        return window(text("Completion %") | color(Pallete::heading1) | bold,
                      input->Render()) |
               color(Pallete::border) | size(WIDTH, EQUAL, 20) |
               size(HEIGHT, EQUAL, 3);
    });

    auto whole = Container::Tab(
        {
            depth_0_renderer,
            depth_1_renderer,
        },
        &depth);

    auto main_renderer = Renderer(whole, [&] {
        Element document = depth_0_renderer->Render();

        if (depth == 1) {
            document = dbox({
                document,
                depth_1_renderer->Render() | clear_under | center,
            });
        }
        return document | size(WIDTH, EQUAL, 114);
    });

    main_renderer |= CatchEvent([](Event event) {
        if (event == Event::Custom) {
            Pallete::setTheme();
            return true;
        }
        return false;
    });
    screen.Loop(main_renderer);

    // the terminal has been restored now
    if (finished) {
        float aggregate_compl{0.0};
        for (float value : compl_levels) {
            if (value == -1)
                continue;
            aggregate_compl += value;
        }

        finishUP(aggregate_compl / tasks_len, compl_levels, indices);
    }
}
