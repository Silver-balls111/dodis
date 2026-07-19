#include <chrono>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <ftxui/ftxui.hpp>
#include <stdexcept>
#include "monthInfo.hpp"

auto error(const char* message){
    return std::runtime_error(message);
}

unsigned int weekdayNo(std::chrono::year_month_day ymd){
    std::chrono::sys_days sd = ymd;
    return std::chrono::weekday(sd).c_encoding();
}

unsigned int weekNo(std::chrono::year_month_day ymd){
    //return the week no given date is, jan1 week is week0
    MonthInfo jan{ymd.year(), std::chrono::January};
    std::chrono::sys_days given_date = ymd;
    int diff = (given_date - jan.start_sys_form).count();
    return (diff + jan.start_weekday_no) / 7;
}

auto weeksInMonth(std::chrono::year year, std::chrono::month month){
    std::chrono::year_month_day start = {year/month/1};
    std::chrono::sys_days s_start = start;
    std::chrono::year_month_day last = {year/std::chrono::month_day_last(month)};
    std::chrono::sys_days s_last = last;

    auto diff = (s_last - s_start).count();
    /*using ceiling is wrong here, we must use +1, because if its exact 35 count
     * after adding the dim days the ceiling won't give correct value. so add 1
     * ex: 2026 may
     */
    return (diff + weekdayNo(start)) / 7 + 1;
}

int main(){
    using namespace ftxui;

    int int_choosen_year = 2026; //should get from user input
    auto choosen_year = std::chrono::year(int_choosen_year);

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
                columns.push_back(text("") | color(Color::Red));   //nerd-font f0c8

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

    std::ifstream file {"contributions.txt"};
    if (!file)
        throw std::runtime_error("error opening file");

    std::stringstream file_buffer;
    file_buffer << file.rdbuf();

    std::vector<int> weeks;
    std::vector<int> day_no;

    for(int i=0;;++i){
        std::chrono::year_month_day ymd;
        float opacity{0.0};
        //consume the whitespace because parserer dosen't. that is the trailing newline.
        file_buffer >> std::ws >> std::chrono::parse("%Y/%m/%d", ymd)>> opacity;

        if(!file_buffer){
            if(file_buffer.eof())
                break;
            else
                error("error reading file");
        }
        auto week_no = weekNo(ymd);
        auto day = weekdayNo(ymd);
        weeks.push_back(week_no);
        day_no.push_back(day);
    }

    //will optimize later
    for(size_t i=0; i<weeks.size(); ++i){
        for(size_t a=0; a<rows.size(); ++a){
            for(size_t b=0; b<rows[a].size(); ++b){
                if(b == weeks[i] && a == day_no[i])
                    rows[a][b] = text("") | color(Color::Blue);
            }
        }
    }

    auto renderer = Renderer([&]{
        return hbox({
            vbox(day) | size(WIDTH, EQUAL, 5),
            vbox({
                hbox(month),
                gridbox(rows),
            }) | hscroll_indicator | xframe
        });
    });


    auto screen = App::TerminalOutput();
    screen.Loop(renderer);

    return 0;
}
