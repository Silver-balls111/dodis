#include <chrono>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color_info.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <ftxui/ftxui.hpp>
#include <stdexcept>
#include "monthInfo.hpp"

namespace fs = std::filesystem;
using namespace ftxui;

Color OpacityToColor(double opacity) {
    if (opacity <= 0.25) return Color::RGB(69, 52, 77);
    if (opacity <= 0.50) return Color::RGB(138, 95, 127);
    if (opacity <= 0.75) return Color::RGB(195, 131, 168);
    return Color::RGB(252, 166, 209);
}

void implementation(const int& year,std::vector<Elements>& rows, std::vector<Element>& month){
    std::string contribution_file_name = std::to_string(year) + ".txt";
    auto choosen_year = std::chrono::year(year);

    unsigned int jan1 = MonthInfo{choosen_year, std::chrono::January}.start_weekday_no;
    unsigned int dec31 = MonthInfo{choosen_year, std::chrono::December}.end_weekday_no;

    rows.clear();
    for(int i=0; i<7; ++i){
        std::vector<Element> columns;
        for(int j=0; j<53; ++j){
            if ((j == 0 && i < jan1) || (j == 52 && i > dec31))
                columns.push_back(text(" "));
            else
                columns.push_back(text("") | color(Color::RGB(128, 128, 128)) );   //nerd-font f0c8
            /* the reason it is box then space is that later when we replace we replace the first match
            * and to replace the box the first match needs to be box not space. while there is a solution
            * even if it was space + block, this is just a lot easier to do.
            */
            columns.push_back(text(" "));
        }
        rows.push_back(columns);
    }

    std::vector<std::string> months {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
        "Aug", "Sep", "Oct", "Nov", "Dec"};

    month.clear();
    for (size_t i=0; i<months.size(); ++i){
        month.push_back(text(months[i]) | bold | color(Color::Red));
        month.push_back(text("    "));      //4 spaces
        int weeks = weeksInMonth(choosen_year, std::chrono::month(i+1));
        if (weeks > 4){
            std::string space(weeks - 4, ' ');
            month.push_back(text(space));
        }
        month.push_back(text(" ")); //to match the initial space of grid
    }

    fs::path base = "contributions";
    std::ifstream file {base / contribution_file_name};
    if (!file)
        throw std::runtime_error("Failed to open 'contribution' file");

    std::stringstream file_buffer;
    bool non_empty_file {true};
    file_buffer << file.rdbuf();
    //if the file is empty this read would have failed so
    if(!file_buffer)
        non_empty_file = false;

    std::vector<int> weeks;
    std::vector<int> day_no;
    std::vector<float> opacitys;

    for(int i=0;non_empty_file;++i){
        std::chrono::year_month_day ymd;
        float opacity{0.0};
        //consume the whitespace because parse() dosen't. that is the trailing newline.
        file_buffer >> std::ws >> std::chrono::parse("%Y/%m/%d", ymd)>> opacity;

        if(!file_buffer){
            if(file_buffer.eof())
                break;
            else
                throw std::runtime_error("Reading file");
        }
        auto week_no = weekNo(ymd);
        auto day = weekdayNo(ymd);
        opacitys.push_back(opacity);
        weeks.push_back(week_no);
        day_no.push_back(day);
    }

    for(size_t i=0; i<weeks.size(); ++i){
        rows[day_no[i]][weeks[i] * 2] = text("") | color(OpacityToColor(opacitys[i]));
    }
}

void Graph(bool interactive){
    try{
        const auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
        std::chrono::year_month_day ymd = today;
        unsigned int int_choosen_year = static_cast<int>(ymd.year());

        std::vector<Elements> rows;

        std::vector<Element> day;
        std::vector<std::string> days {"   ", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; //initial space it will be a vbox item
        for(auto x: days){
            day.push_back(text(x) | bold | color(Color::Red));
        }

        std::vector<Element> month;

        implementation(int_choosen_year, rows, month);

        fs::path base = "contributions";

        if (interactive){
            std::vector<int> file_names;
            for(const auto& entry: fs::directory_iterator(base)){
                if (entry.is_regular_file() && entry.path().extension() == fs::path(".txt")){
                    try{
                        int x = std::stoi(entry.path().stem());
                        std::chrono::year yr{x};
                        if(yr.ok())
                            file_names.push_back(x);
                    }
                    catch(std::invalid_argument){ continue;}
                    catch(std::out_of_range){ continue; }
                }
            }
            std::ranges::sort(file_names, std::greater<int>());         //descending order
            auto style = ButtonOption::Animated(Color::Default, Color::Grey62,
                                                Color::Default, Color::OrangeRed1);

            auto buttons = Container::Vertical({});
            auto onYearSelected = [&](int file) {
                int_choosen_year = file;
                implementation(int_choosen_year, rows, month);
            };

            for (int file : file_names) {
                buttons->Add(Button(std::to_string(file), [&onYearSelected, file] { onYearSelected(file); }, style));
            }

            auto renderer = Renderer(buttons, [&]{
                return hbox({
                    vbox(day) | size(WIDTH, EQUAL, 5),
                    vbox({
                        hbox(month),
                        gridbox(rows),
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
        }
        else{
            auto frame = hbox({
                vbox(day) | size(WIDTH, EQUAL, 5),
                vbox({
                    hbox(month),
                    gridbox(rows),
                }) | hscroll_indicator | xframe
            });

            auto screen = Screen::Create(Dimension::Fit(frame));
            Render(screen, frame);
            screen.Print();
        }
    }
    catch(std::runtime_error const& e){
        std::cerr<<"Error: "<<e.what();
        return;
    }
}
