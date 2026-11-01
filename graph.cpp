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
namespace chrono = std::chrono;
using namespace ftxui;

// BUG: hovering from bottom tile to above dosen't update tooltip
// NOTE: on exit from tile sets tooltip to empty, is it fine? 
// or do we show the average?

std::string getSuffixedDate(int day) {
    switch (day) {
    case 1:
    case 21:
    case 31:
        return std::to_string(day) + "st";
    case 2:
    case 22:
        return std::to_string(day) + "nd";
    case 3:
    case 23:
        return std::to_string(day) + "rd";
    default:
        return std::to_string(day) + "th";
    }
}

Color opacityToColor(double opacity) {
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

void rebuildGrid(Component &horC, std::vector<Component> &columns) {
    horC->DetachAllChildren();
    for (int weeks = 0; weeks < 2 * 54; ++weeks) {
        auto verC = Container::Vertical({});
        for (int day = 0; day < 7; ++day) {
            verC->Add(columns[7 * weeks + day]);
        }
        horC->Add(verC);
    }
}

Component blank() {
    return Renderer([] { return text(" "); });
}

Component hovb(const std::string label, std::string &ddate,
               Color blockc = Color::RGB(128, 128, 128)) {
    auto block = Renderer([=] { return text("") | color(blockc); });

    return Hoverable(
        block, [&, label] { ddate = label; }, [&] { ddate = " "; });
}

void draw(const int year, std::vector<Component> &columns, std::string &ddate) {

    chrono::year choosen_year{year};
    unsigned int jan1 = weekdayNo(choosen_year / chrono::January / 1);
    unsigned int dec31 = weekdayNo(choosen_year / chrono::December / 31);

    columns.clear();
    int wks{53};
    if (choosen_year.is_leap() && jan1 == 6) {
        wks = 54;
    }
    for (int week = 0; week < wks; ++week) {
        for (int day = 0; day < 7; ++day) {
            if ((week == 0 && day < jan1) || (week == wks - 1 && day > dec31))
                columns.push_back(blank());
            else {
                chrono::year_month_day date =
                    dayInYear(choosen_year, (7 * week + day) - jan1);
                std::string label = std::format(
                    "No task done on {:%A}, {:%b} {}, {}.",
                    chrono::weekday(day), date.month(),
                    getSuffixedDate(static_cast<unsigned>(date.day())),
                    choosen_year);
                columns.push_back(hovb(label, ddate));
            }
        }
        for (int i = 0; i < 7; ++i)
            columns.push_back(blank());
    }
    if (wks == 53) {
        for (int i = 0; i < 14; ++i)
            columns.push_back(blank());
    }

    std::string contrib_file_name =
        std::format("contributions/{}.txt", choosen_year);
    std::ifstream file{contrib_file_name};
    if (!file)
        throw std::runtime_error(
            std::format("Opening file {}.txt", choosen_year));

    std::stringstream file_buffer;
    file_buffer << file.rdbuf();
    file_buffer.clear();

    std::vector<int> week_no;
    std::vector<int> day_no;
    std::vector<double> opacitys;

    chrono::year_month_day ymd;
    float opacity{0.0};

    while (file_buffer >> std::ws >> chrono::parse("%Y/%m/%d", ymd) >>
           opacity) {
        opacitys.push_back(opacity);
        week_no.push_back(weekNumYear(ymd));
        day_no.push_back(weekdayNo(ymd));
    }

    if (!file_buffer.eof())
        throw std::runtime_error(
            std::format("Reading file {}.txt", choosen_year));

    double sum{0};
    if (!opacitys.empty()) {
        for (double opacity : opacitys)
            sum += opacity;
    }
    ddate = std::format("Average rate of completion for {} is {:.3}%.", choosen_year,
                        sum / 365 * 100);

    for (size_t i = 0; i < week_no.size(); ++i) {
        chrono::year_month_day date =
            dayInYear(choosen_year, (7 * week_no[i] + day_no[i]) - jan1);

        std::string op_label =
            std::format("{:.3}% completion", opacitys[i] * 100);
        if (opacitys[i] == 0)
            op_label = "No task done";
        std::string label = std::format(
            "{} on {:%A}, {:%b} {}, {}.", op_label, chrono::weekday(day_no[i]),
            date.month(), getSuffixedDate(static_cast<unsigned>(date.day())),
            date.year());

        columns[7 * 2 * week_no[i] + day_no[i]] =
            hovb(label, ddate, opacityToColor(opacitys[i]));
    }
}

void Graph(bool interactive) {
    try {
        const auto today =
            chrono::floor<std::chrono::days>(chrono::system_clock::now());
        chrono::year_month_day ymd = today;

        unsigned int int_choosen_year = static_cast<int>(ymd.year());

        std::vector<Component> columns;

        std::vector<Element> day;
        std::vector<std::string> days{"   ", "Sun", "Mon", "Tue",
                                      "Wed", "Thu", "Fri", "Sat"};
        for (auto x : days) {
            day.push_back(text(x) | bold | color(Color::Red));
        }

        std::vector<Element> month;
        std::vector<std::string> months{"Jan", "Feb", "Mar", "Apr",
                                        "May", "Jun", "Jul", "Aug",
                                        "Sep", "Oct", "Nov", "Dec"};
        for (size_t i = 0; i < months.size(); ++i) {
            month.push_back(text("  ")); // 2 spaces
            month.push_back(text(months[i]) | bold | color(Color::Red));
            month.push_back(text("  ")); // 2 spaces
            if (i % 2 == 0) {
                std::string space(" ");
                month.push_back(text(space));
            }
            month.push_back(text(" ")); // to match the initial space of grid
        }

        if (interactive) {
            std::vector<int> file_names;
            for (const auto &entry : fs::directory_iterator("contributions")) {
                if (entry.is_regular_file() &&
                    entry.path().extension() == fs::path(".txt")) {
                    try {
                        int x = std::stoi(entry.path().stem());
                        chrono::year yr{x};
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

            std::string ddate;

            auto horC = Container::Horizontal({});
            draw(file_names[0], columns, ddate);
            rebuildGrid(horC, columns);

            auto onYearSelected = [&](int file) {
                draw(file, columns, ddate);
                rebuildGrid(horC, columns);
            };

            for (int file : file_names) {
                buttons->Add(Button(
                    std::to_string(file),
                    [&onYearSelected, file] { onYearSelected(file); }, style));
            }

            float scroll_x = 0.1f;
            float scroll_y = 0.1f;

            SliderOption<float> option_x;
            option_x.value = &scroll_x;
            option_x.min = 0.f;
            option_x.max = 1.f;
            option_x.increment = 0.1f;
            option_x.label = " ";
            option_x.direction = Direction::Right;
            option_x.color_active = Color::Blue;
            option_x.color_inactive = Color::BlueLight;

            auto scrollbar_x = Slider(option_x);

            auto dayC =
                Renderer([&] { return vbox(day) | size(WIDTH, EQUAL, 5); });
            auto monthC = Renderer([&] { return hbox(month); });
            auto date_box = Renderer([&] {
                return hbox({text(ddate), filler(), text("Less "),
                             text(" ") | color(opacityToColor(0)),
                             text(" ") | color(opacityToColor(0.25)),
                             text(" ") | color(opacityToColor(0.5)),
                             text(" ") | color(opacityToColor(0.75)),
                             text(" ") | color(opacityToColor(1)),
                             text("More ")}) |
                       borderRounded;
            });

            auto content = Container::Horizontal(
                {Container::Vertical({dayC}),
                 Container::Vertical({monthC, horC | size(HEIGHT, EQUAL, 7)})});

            // can't simply apply focusPositionRelative to content it must be
            // rendered first then only panning will work don't know why though.
            auto scroller = Renderer(content, [&] {
                return content->Render() |
                       focusPositionRelative(scroll_x, scroll_y) | frame | flex;
            });
            auto screen = App::TerminalOutput();
            auto whole = Container::Horizontal(
                {Container::Vertical({scroller, scrollbar_x | Maybe([&] {
                                                    return screen.dimx() < 120;
                                                }),
                                      date_box | xframe}) |
                     flex,
                 buttons | vscroll_indicator | yframe |
                     size(HEIGHT, LESS_THAN, 8)});

            auto renderer = Renderer(whole, [&] {
                return hbox({
                    whole->Render(),
                });
            });

            renderer |= CatchEvent([&](Event event) {
                if (event == Event::Character('q') || event == Event::Escape) {
                    screen.ExitLoopClosure()();
                    return true;
                }
                return false;
            });
            screen.Loop(renderer);
        } else {
            // draw(int_choosen_year, columns);
            auto frame = hbox({vbox(day) | size(WIDTH, EQUAL, 5),
                               vbox({
                                   hbox(month),
                                   // vflow(columns) | size(HEIGHT, EQUAL, 7),
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
