#include "monthInfo.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <ftxui/dom/elements.hpp>
#include <ftxui/ftxui.hpp>
#include <ftxui/screen/color_info.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace ftxui;

Color OpacityToColor(double opacity) {
    if (opacity == 0)
        return Color::RGB(128, 128, 128);
    if (opacity <= 0.25)
        return Color::RGB(69, 52, 77);
    if (opacity <= 0.50)
        return Color::RGB(138, 95, 127);
    if (opacity <= 0.75)
        return Color::RGB(195, 131, 168);
    return Color::RGB(252, 166, 209);
}

// FIX: change to chrono::year int not needed
void implementation(const int year, std::vector<Element> &columns,
                    std::vector<Element> &month) {
    auto choosen_year = std::chrono::year(year);

    // TODO: no need to create MonthInfo obj just call fun
    unsigned int jan1 =
        MonthInfo{choosen_year, std::chrono::January}.start_weekday_num;
    unsigned int dec31 =
        MonthInfo{choosen_year, std::chrono::December}.end_weekday_num;

    columns.clear();
    for (int week = 0; week < 53; ++week) {
        for (int day = 0; day < 7; ++day) {
            if ((week == 0 && day < jan1) || (week == 52 && day > dec31))
                columns.push_back(text(" "));
            else
                columns.push_back(
                    text("") |
                    color(Color::RGB(128, 128, 128))); // nerd-font f0c8
        }
        for (int i = 0; i < 7; ++i)
            columns.push_back(text(" "));
    }

    // since gridbox fill horizontally first then vertical we would get 53 rows
    // with 7 columns each thus the need to use vflow.
    /*
    weeks.clear();
    for (int week = 0; week < 53; ++week) {
        std::vector<Element> days;
        for (int day = 0; day < 7; ++day) {
            if ((week == 0 && day < jan1) || (week == 52 && day > dec31))
                days.push_back(text(" "));
            else
                days.push_back(
                    text("") |
                    color(Color::RGB(128, 128, 128))); // nerd-font f0c8
            days.push_back(text("A"));
        }
        weeks.push_back(days);
    }
    */

    std::vector<std::string> months{"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    month.clear();
    for (size_t i = 0; i < months.size(); ++i) {
        month.push_back(text("  ")); // 2 spaces
        month.push_back(text(months[i]) | bold | color(Color::Red));
        month.push_back(text("  ")); // 2 spaces
        // int weeks = weeksInMonth(choosen_year, std::chrono::month(i + 1));
        //  if (weeks > 4){
        //      //std::string space(2 * (weeks - 4), ' ');
        //      std::string space(" ");
        //      month.push_back(text(space));
        //  }
        if (i % 2 == 0) {
            std::string space(" ");
            month.push_back(text(space));
        }
        month.push_back(text(" ")); // to match the initial space of grid
    }

    // FIX: use format string to get file name
    std::string contrib_file_name =
        "contributions/" + std::to_string(year) + ".txt";
    std::filesystem::path contrib_file_path{contrib_file_name};
    std::ifstream file{contrib_file_path};
    if (!file)
        // TODO: returnt the file name in error msg 2026..
        throw std::runtime_error("Failed to open 'contribution' file");

    // if the file is empty this read would have failed so
    bool non_empty_file = !std::filesystem::is_empty(contrib_file_path);
    std::stringstream file_buffer;
    file_buffer << file.rdbuf();

    std::vector<int> week_no;
    std::vector<int> day_no;
    std::vector<double> opacitys;

    // FIX: remove dependency on non_empty_file
    for (int i = 0; non_empty_file; ++i) {
        std::chrono::year_month_day ymd;
        float opacity{0.0};
        // consume the whitespace because parse() dosen't i.e. the trailing newline.
        file_buffer >> std::ws >> std::chrono::parse("%Y/%m/%d", ymd) >>
            opacity;

        if (!file_buffer) {
            if (file_buffer.eof())
                break;
            else
                throw std::runtime_error("Reading file");
        }
        opacitys.push_back(opacity);
        week_no.push_back(weekNumYear(ymd));
        day_no.push_back(weekdayNo(ymd));
    }

    for (size_t i = 0; i < week_no.size(); ++i) {
        // HACK: for range check switch to [] later.
        columns.at(7 * 2 * week_no[i] + day_no[i]) =
            text("") | color(OpacityToColor(opacitys[i]));
    }
}

void Graph(bool interactive) {
    try {
        const auto today = std::chrono::floor<std::chrono::days>(
            std::chrono::system_clock::now());
        std::chrono::year_month_day ymd = today;
        unsigned int int_choosen_year = static_cast<int>(ymd.year());

        std::vector<Element> columns;

        std::vector<Element> day;
        // initial space it will be a vbox item
        std::vector<std::string> days{"   ", "Sun", "Mon", "Tue",
                                      "Wed", "Thu", "Fri", "Sat"};
        for (auto x : days) {
            day.push_back(text(x) | bold | color(Color::Red));
        }

        std::vector<Element> month;

        implementation(int_choosen_year, columns, month);

        if (interactive) {
            std::vector<int> file_names;
            for (const auto &entry : fs::directory_iterator("contributions")) {
                if (entry.is_regular_file() &&
                    entry.path().extension() == fs::path(".txt")) {
                    try {
                        int x = std::stoi(entry.path().stem());
                        std::chrono::year yr{x};
                        if (yr.ok())
                            file_names.push_back(x);
                    } catch (std::invalid_argument) {
                        continue;
                    } catch (std::out_of_range) {
                        continue;
                    }
                }
            }
            std::ranges::sort(file_names,
                              std::greater<int>()); // descending order
            auto style =
                ButtonOption::Animated(Color::Default, Color::Grey62,
                                       Color::Default, Color::OrangeRed1);

            auto buttons = Container::Vertical({});
            auto onYearSelected = [&](int file) {
                int_choosen_year = file;
                implementation(int_choosen_year, columns, month);
            };

            for (int file : file_names) {
                buttons->Add(Button(
                    std::to_string(file),
                    [&onYearSelected, file] { onYearSelected(file); }, style));
            }

            auto renderer = Renderer(buttons, [&] {
                return hbox({
                    vbox(day) | size(WIDTH, EQUAL, 5),
                    vbox({
                        hbox(month),
                        vflow(columns) | size(HEIGHT, EQUAL, 7),
                    }),
                    buttons->Render() | vscroll_indicator | frame |
                        size(HEIGHT, LESS_THAN, 8),
                });
            });

            auto screen = App::TerminalOutput();
            renderer |= CatchEvent([&](Event event) {
                if (event == Event::Character('q')) {
                    screen.ExitLoopClosure()();
                    return true;
                }
                return false;
            });
            screen.Loop(renderer);
        } else {
            auto frame = hbox({vbox(day) | size(WIDTH, EQUAL, 5),
                               vbox({
                                   hbox(month),
                                   vflow(columns) | size(HEIGHT, EQUAL, 7),
                               }) | hscroll_indicator |
                                   xframe});

            auto screen = Screen::Create(Dimension::Fit(frame));
            Render(screen, frame);
            screen.Print();
        }
    } catch (std::runtime_error const &e) {
        std::cerr << "Error: " << e.what();
        return;
    }
}
