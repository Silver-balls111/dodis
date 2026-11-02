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

std::string getSuffixedDate(unsigned int day) {
    std::string suffix{};
    switch (day) {
    case 1:
    case 21:
    case 31:
        suffix = "st";
        break;
    case 2:
    case 22:
        suffix = "nd";
        break;
    case 3:
    case 23:
        suffix = "rd";
        break;
    default:
        suffix = "th";
    }
    return std::to_string(day) + suffix;
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

void rebuildGrid(Component &hbox_comp, std::vector<Component> &tiles) {
    hbox_comp->DetachAllChildren();
    for (int weeks = 0; weeks < 2 * 54; ++weeks) {
        auto column = Container::Vertical({});
        for (int day = 0; day < 7; ++day) {
            column->Add(tiles[7 * weeks + day]);
        }
        hbox_comp->Add(column);
    }
}

Component getEmptyTile() {
    return Renderer([] { return text(" "); });
}

Component getHoverableTile(std::string &tooltip_content,
                           const std::string on_enter_text,
                           Color tile_color = Color::RGB(128, 128, 128)) {
    auto block = Renderer(
        [=] { return text("") | color(tile_color); }); // nerd-font f0c8

    return Hoverable(
        block, [&, on_enter_text] { tooltip_content = on_enter_text; },
        [&] { tooltip_content = " "; });
}

void rebuildTiles(const int year, std::vector<Component> &tiles,
                  std::string &tooltip_content) {
    chrono::year choosen_year{year};
    unsigned int jan1 = weekDayNum(choosen_year / chrono::January / 1);
    unsigned int dec31 = weekDayNum(choosen_year / chrono::December / 31);

    tiles.clear();

    // leap yr which starts at Sat ends at Sun has 54 wks. ex: 2028
    int weeks = choosen_year.is_leap() && jan1 == 6 ? 54 : 53;

    for (int week = 0; week < weeks; ++week) {
        for (int day = 0; day < 7; ++day) {
            if ((week == 0 && day < jan1) || (week == weeks - 1 && day > dec31))
                tiles.push_back(getEmptyTile());
            else {
                chrono::year_month_day date =
                    dayInYear(choosen_year, (7 * week + day) - jan1);
                std::string label = std::format(
                    "No task done on {:%A}, {:%b} {}, {}.",
                    chrono::weekday(day), date.month(),
                    getSuffixedDate(static_cast<unsigned>(date.day())),
                    choosen_year);

                tiles.push_back(getHoverableTile(tooltip_content, label));
            }
        }
        for (int i = 0; i < 7; ++i)
            tiles.push_back(getEmptyTile());
    }
    // to avoid resizing of yr button column between leap and non-leap yrs.
    if (weeks == 53) {
        for (int i = 0; i < 14; ++i)
            tiles.push_back(getEmptyTile());
    }

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

    std::vector<int> week_num;
    std::vector<int> day_num;
    std::vector<double> opacitys;

    chrono::year_month_day ymd;
    float opacity{0.0};

    // need to consume whitespace because parse() dosen't or else it would cause
    // error for the trailing newline.
    while (file_buffer >> std::ws >> chrono::parse("%Y/%m/%d", ymd) >>
           opacity) {
        week_num.push_back(weekNumInYear(ymd));
        day_num.push_back(weekDayNum(ymd));
        opacitys.push_back(opacity);
    }

    if (!file_buffer.eof())
        throw std::runtime_error(
            std::format("Reading file {}.txt", choosen_year));

    double sum{0};
    for (double opacity : opacitys)
        sum += opacity;

    tooltip_content =
        std::format("Average rate of completion for {} is {:.3}%.",
                    choosen_year, sum / 365 * 100);

    for (size_t i = 0; i < week_num.size(); ++i) {
        chrono::year_month_day date =
            dayInYear(choosen_year, (7 * week_num[i] + day_num[i]) - jan1);
        std::string op_label =
            std::format("{:.3}% completion", opacitys[i] * 100);
        if (opacitys[i] == 0)
            op_label = "No task done";
        std::string label = std::format(
            "{} on {:%A}, {:%b} {}, {}.", op_label, chrono::weekday(day_num[i]),
            date.month(), getSuffixedDate(static_cast<unsigned>(date.day())),
            date.year());

        tiles[7 * 2 * week_num[i] + day_num[i]] = getHoverableTile(
            tooltip_content, label, opacityToColor(opacitys[i]));
    }
}

void Graph(bool interactive) {
    try {
        std::vector<Component> tiles;

        std::vector<Element> day;
        std::vector<std::string> days{"   ", "Sun", "Mon", "Tue",
                                      "Wed", "Thu", "Fri", "Sat"};
        for (auto x : days) {
            day.push_back(text(x) | bold | color(Color::Red));
        }
        auto day_comp =
            Renderer([&] { return vbox(day) | size(WIDTH, EQUAL, 5); });

        std::vector<Element> month;
        std::vector<std::string> months{"Jan", "Feb", "Mar", "Apr",
                                        "May", "Jun", "Jul", "Aug",
                                        "Sep", "Oct", "Nov", "Dec"};
        for (size_t i = 0; i < months.size(); ++i) {
            month.push_back(text("  ")); // 2 spaces
            month.push_back(text(months[i]) | bold | color(Color::Red));
            month.push_back(text("  ")); // 2 spaces
            if (i % 2 == 0) {
                month.push_back(text(" "));
            }
            month.push_back(text(" "));
        }
        auto month_comp = Renderer([&] { return hbox(month); });

        std::string tooltip_content;
        auto tooltip = Renderer([&] {
            return hbox({text(tooltip_content), filler(), text("Less "),
                         text(" ") | color(opacityToColor(0)),
                         text(" ") | color(opacityToColor(0.25)),
                         text(" ") | color(opacityToColor(0.5)),
                         text(" ") | color(opacityToColor(0.75)),
                         text(" ") | color(opacityToColor(1)),
                         text("More ")}) |
                   borderRounded;
        });

        auto hbox_comp = Container::Horizontal({});
        auto content = Container::Horizontal(
            {Container::Vertical({day_comp}),
             Container::Vertical(
                 {month_comp, hbox_comp | size(HEIGHT, EQUAL, 7)})});

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

            rebuildTiles(file_names[0], tiles, tooltip_content);
            rebuildGrid(hbox_comp, tiles);

            auto style =
                ButtonOption::Animated(Color::Default, Color::Grey62,
                                       Color::Default, Color::OrangeRed1);
            auto buttons = Container::Vertical({});

            auto onYearSelected = [&](int file) {
                rebuildTiles(file, tiles, tooltip_content);
                rebuildGrid(hbox_comp, tiles);
            };

            for (int file : file_names) {
                buttons->Add(Button(
                    std::to_string(file),
                    [&onYearSelected, file] { onYearSelected(file); }, style));
            }

            float scroll_pos_x = 0.1f;
            float scroll_pos_y = 0.1f;

            SliderOption<float> option_x;
            option_x.value = &scroll_pos_x;
            option_x.min = 0.f;
            option_x.max = 1.f;
            option_x.increment = 0.1f;
            option_x.label = " ";
            option_x.direction = Direction::Right;
            option_x.color_active = Color::Blue;
            option_x.color_inactive = Color::BlueLight;

            // NOTE: option.lable is only available in my fork of ftxui
            auto scrollbar_x = Slider(option_x);

            // panning with focusPositionRelative requires knowing the actual
            // rendered box size of the content to compute the offsets, so we
            // must first render it to make it scrollable.
            auto scrollable_content = Renderer(content, [&] {
                return content->Render() |
                       focusPositionRelative(scroll_pos_x, scroll_pos_y) |
                       frame | flex;
            });

            /*
            here we had to use screen.dimx() instead of Terminal::Size().dimx
            because term size only reports the initial size where as screen size
            is dynamic. https://github.com/ArthurSonzogni/FTXUI/discussions/1047
            dimx/y is in terms of characters, do / 10 on pixels
            */
            // BUG: yr selector buttons are not focused initially
            auto screen = App::TerminalOutput();
            auto whole = Container::Horizontal(
                {Container::Vertical(
                     {scrollable_content,
                      scrollbar_x | Maybe([&] { return screen.dimx() < 120; }),
                      tooltip | xframe}) |
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
            const auto today =
                chrono::floor<std::chrono::days>(chrono::system_clock::now());
            chrono::year_month_day ymd = today;
            unsigned int current_year = static_cast<int>(ymd.year());

            rebuildTiles(current_year, tiles, tooltip_content);
            rebuildGrid(hbox_comp, tiles);

            auto whole = Container::Horizontal(
                {Container::Vertical(
                     {content | xframe | flex,
                      tooltip})
            }) ;

            auto whole_rendered = whole->Render();
            auto screen = Screen::Create(Dimension::Fit(whole_rendered, true));
            Render(screen, whole_rendered);
            screen.Print();
        }
    } catch (std::runtime_error const &e) {
        std::cerr << "Error: " << e.what();
        return;
    }
}
