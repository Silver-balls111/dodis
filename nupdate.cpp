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

namespace CompColor {
const static inline ftxui::Color done = ftxui::Color::RGB(166, 227, 161);
const static inline ftxui::Color not_done = ftxui::Color::RGB(255, 180, 171);
}; // namespace CompColor

ftxui::Color lerpColor(double t) { // t in [0,1]
    auto lerp = [](int a, int b, double t) {
        return static_cast<uint8_t>(a + (b - a) * t);
    };
    return ftxui::Color::RGB(lerp(255, 166, t), lerp(180, 227, t),
                             lerp(171, 161, t));
}

namespace Glyph {
static constexpr std::string_view empty{"   "};
static constexpr std::string_view check{"  "};
static constexpr std::string_view cross{"  "};
static constexpr std::string_view below12{"󰝦  "};
static constexpr std::string_view below25{"󰪞  "};
static constexpr std::string_view below37{"󰪟  "};
static constexpr std::string_view below50{"󰪠  "};
static constexpr std::string_view below62{"󰪡  "};
static constexpr std::string_view below75{"󰪢  "};
static constexpr std::string_view below87{"󰪣  "};
static constexpr std::string_view below100{"󰪤  "};
}; // namespace Glyph

std::string_view opacityToGlyph(float op) {
    using namespace Glyph;
    if (op == 0)
        return cross;
    else if (op < 0.12)
        return below12;
    else if (op < 0.25)
        return below25;
    else if (op < 0.37)
        return below37;
    else if (op < 0.50)
        return below50;
    else if (op < 0.62)
        return below62;
    else if (op < 0.75)
        return below75;
    else if (op < 0.87)
        return below87;
    else if (op < 1)
        return below100;
    else
        return check;
}

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

// TODO: store entered completion value on task which resets everyday
// TODO: add colors to percentage level from red green gradient
// TODO: sort task on completion rate on startup

void Update() {
    using namespace ftxui;

    auto screen = App::TerminalOutput();
    std::ifstream file{"tasks.txt"};

    std::vector<std::string> tasks;

    std::string task;
    while (getline(file, task)) {
        tasks.push_back(task);
    }
    file.close();

    const int vec_sizes = tasks.size();
    std::vector<double> comp_level;
    comp_level.reserve(vec_sizes);
    std::vector<std::string_view> icon;
    icon.reserve(vec_sizes);
    for (int i = 0; i < vec_sizes; ++i) {
        comp_level.emplace_back(-1);
        icon.emplace_back(Glyph::empty);
    }

    MenuOption opt;
    opt.entries_option.transform = [&](EntryState state) {
        const double level = comp_level[state.index];
        const bool resolved = (level == 0.0 || level == 1.0);
        const bool untouched = (level < 0);
        const Color grey_blue = Color::RGB(148, 158, 166);

        Color fg;
        if (untouched)
            fg = state.focused ? grey_blue : Color::RGB(255, 255, 255);
        else
            fg = lerpColor(level);

        Element gutter = paragraph(icon[state.index]) | color(fg);
        Element label = paragraph(state.label) | color(fg);
        if (resolved)
            label |= strikethrough;

        auto hi = hbox({gutter, label});
        if (resolved)
            hi |= dim;
        if (state.focused)
            hi = hi | color(fg) | inverted;
        return hi;
    };

    int selected = 0;
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
            if (input_text.ends_with('%'))
                input_text = input_text.substr(0, input_text.size() - 1);
            v = std::stof(input_text);
            if (v < 0)
                v = 0;
            else if (v > 100)
                v = 1;
            else
                v = v / 100;
            comp_level[selected] = v;
            icon[selected] = opacityToGlyph(v);
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
            comp_level[selected] = comp_level[selected] == 1 ? -1 : 1;
            icon[selected] =
                comp_level[selected] == 1 ? Glyph::check : Glyph::empty;
            return true;
        }

        if (event == Event::x) {
            comp_level[selected] = comp_level[selected] == 0 ? -1 : 0;
            icon[selected] =
                comp_level[selected] == 0 ? Glyph::cross : Glyph::empty;
            return true;
        }

        if (event == Event::p) {
            depth = 1;
            return true;
        }

        if (event == Event::Escape || event == Event::q) {
            float aggregate_compl{0.0};
            for (float value : comp_level)
                aggregate_compl += value;
            finishUP(aggregate_compl / tasks.size());
            screen.ExitLoopClosure()();
        }
        return false;
    });

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
        return vbox({task_renderer->Render(), tooltip}) |
               size(WIDTH, EQUAL, 114);
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
