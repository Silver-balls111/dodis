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

void Graph(bool interactive){
    using namespace ftxui;

    try{
        const auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
        std::chrono::year_month_day ymd = today;
        unsigned int int_choosen_year = static_cast<int>(ymd.year()); //should get from user input
        unsigned int* ptr = &int_choosen_year;
        std::string contribution_file_name = std::to_string(*ptr) + ".txt";
        auto choosen_year = std::chrono::year(*ptr);

        unsigned int jan1 = MonthInfo{choosen_year, std::chrono::January}.start_weekday_no;
        unsigned int dec31 = MonthInfo{choosen_year, std::chrono::December}.end_weekday_no;

        //Elements row_item; much more apparent later
        std::vector<Elements> rows;
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

        std::vector<Element> day;
        std::vector<std::string> days {"   ", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; //initial space it will be a vbox item
        for(auto x: days){
            day.push_back(text(x) | bold | color(Color::Red));
        }

        std::vector<Element> month;
        std::vector<std::string> months {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
            "Aug", "Sep", "Oct", "Nov", "Dec"};

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
        file_buffer << file.rdbuf();

        std::vector<int> weeks;
        std::vector<int> day_no;
        std::vector<float> opacitys;

        for(int i=0;;++i){
            std::chrono::year_month_day ymd;
            float opacity{0.0};
            //consume the whitespace because parserer dosen't. that is the trailing newline.
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

        //(69, 52, 77, 1)()((138, 95, 127, 1))(195, 131, 168, 1)(252,166,209,1)
        //will optimize later
        for(size_t i=0; i<weeks.size(); ++i){
            for(size_t a=0; a<rows.size(); ++a){
                for(size_t b=0; b<rows[a].size(); ++b){
                    if(b == weeks[i] * 2 && a == day_no[i]){
                        if(opacitys[i] > 0 && opacitys[i] <= 0.25)
                            rows[a][b] = text("") | color(Color::RGB(69, 52, 77));
                        else if(opacitys[i] > 0.25 && opacitys[i] <= 0.50)
                            rows[a][b] = text("") | color(Color::RGB(138, 95, 127));
                        else if(opacitys[i] > 0.50 && opacitys[i] <= 0.75)
                            rows[a][b] = text("") | color(Color::RGB(195, 131, 168));
                        else
                            rows[a][b] = text("") | color(Color::RGB(252, 166, 209));
                    }
                }
            }
        }

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
            auto style = ButtonOption::Animated(Color::Default, Color::Blue,
                                                Color::Default, Color::Red);

            auto buttons = Container::Vertical({});
            for(int file: file_names){
                auto button = Button( std::to_string(file), [&int_choosen_year, file]{ int_choosen_year = file;}, style);
                buttons->Add(button);
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
                    text(std::to_string(int_choosen_year)),
                    text(contribution_file_name),
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
