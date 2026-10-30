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

void draw(const int year, std::vector<Element> &columns) {

    chrono::year choosen_year{year};
    unsigned int jan1 = weekdayNo(choosen_year / chrono::January / 1);
    unsigned int dec31 = weekdayNo(choosen_year / chrono::December / 31);

    columns.clear();
    int wks{53};
    // leap yr which starts at Sat end with Sun has 54 weeks ex: 2028
    if (choosen_year.is_leap() && jan1 == 6) {
        wks = 54;
    }
    for (int week = 0; week < wks; ++week) {
        for (int day = 0; day < 7; ++day) {
            if ((week == 0 && day < jan1) || (week == wks - 1 && day > dec31))
                columns.push_back(text(" "));
            else
                columns.push_back(
                    text("") |
                    color(Color::RGB(128, 128, 128))); // nerd-font f0c8
        }
        for (int i = 0; i < 7; ++i)
            columns.push_back(text(" "));
    }
    // to avoid resizing of buttons row between leap and non-leap yrs.
    if (wks == 53) {
        for (int i = 0; i < 14; ++i)
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

    std::string contrib_file_name =
        std::format("contributions/{}.txt", choosen_year);
    std::ifstream file{contrib_file_name};
    if (!file)
        throw std::runtime_error(
            std::format("Opening file {}.txt", choosen_year));

    std::stringstream file_buffer;
    file_buffer << file.rdbuf();
    // if the file was empty then it would set file_buffer to fail state instead
    // of setting its eof bit and program would throw error
    file_buffer.clear();

    std::vector<int> week_no;
    std::vector<int> day_no;
    std::vector<double> opacitys;

    chrono::year_month_day ymd;
    float opacity{0.0};

    // need to consume whitespace because parse() dosen't or else it would cause
    // error for the trailing newline
    while (file_buffer >> std::ws >> chrono::parse("%Y/%m/%d", ymd) >>
           opacity) {
        opacitys.push_back(opacity);
        week_no.push_back(weekNumYear(ymd));
        day_no.push_back(weekdayNo(ymd));
    }

    if (!file_buffer.eof())
        throw std::runtime_error(
            std::format("Reading file {}.txt", choosen_year));

    for (size_t i = 0; i < week_no.size(); ++i) {
        columns[7 * 2 * week_no[i] + day_no[i]] =
            text("") | color(OpacityToColor(opacitys[i]));
    }
}

void Graph(bool interactive) {
    try {
        const auto today =
            chrono::floor<std::chrono::days>(chrono::system_clock::now());
        chrono::year_month_day ymd = today;

        // only reason we need to convert year to int is that later we need
        // to sort file names and we can't we chrono::year there.
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
            auto onYearSelected = [&](int file) { draw(file, columns); };

            for (int file : file_names) {
                buttons->Add(Button(
                    std::to_string(file),
                    [&onYearSelected, file] { onYearSelected(file); }, style));
            }

            float scroll_x = 0.1f;
            float scroll_y = 0.1f;

            auto content = Renderer([&] {
                return hbox({vbox(day) | size(WIDTH, EQUAL, 5),
                             vbox({
                                 hbox(month),
                                 vflow(columns) | size(HEIGHT, EQUAL, 7),
                             })});
            });

            auto scrollable_content = Renderer(content, [&] {
                return content->Render() |
                       focusPositionRelative(scroll_x, scroll_y) | xframe | flex;
            });
            
            SliderOption<float> option_x;
            option_x.value = &scroll_x;
            option_x.min = 0.f;
            option_x.max = 1.f;
            option_x.increment = 0.1f;
            option_x.direction = Direction::Right;
            option_x.color_active = Color::Blue;
            option_x.color_inactive = Color::BlueLight;

            auto scrollbar_x = Slider(option_x);

            auto whole = Container::Horizontal({
                Container::Vertical({
                    scrollable_content,
                    scrollbar_x,
                }) | flex,
                buttons | vscroll_indicator | yframe |
                        size(HEIGHT, LESS_THAN, 8),
            });

            draw(file_names[0], columns);
            auto renderer = Renderer(whole, [&] {
                return hbox({
                    whole->Render(),
                });
            });

            auto screen = App::TerminalOutput();
            renderer |= CatchEvent([&](Event event) {
                if (event == Event::Character('q') || event == Event::Escape) {
                    screen.ExitLoopClosure()();
                    return true;
                }
                return false;
            });
            screen.Loop(renderer);
        } else {
            draw(int_choosen_year, columns);
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
