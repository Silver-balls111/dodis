#include <chrono>
#include <cmath>
#include <ftxui/ftxui.hpp>
#include "monthInfo.hpp"

unsigned int weekday_no(std::chrono::year_month_day ymd){
    std::chrono::sys_days sd = ymd;
    return std::chrono::weekday(sd).c_encoding();
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
    return (diff + weekday_no(start)) / 7 + 1;
}

int main(){
    using namespace ftxui;

    int int_choosen_year = 2026; //should get from user input
    auto choosen_year = std::chrono::year(int_choosen_year);

    unsigned int jan1 = MonthInfo{std::chrono::January}.start_weekday_no;
    unsigned int dec31 = MonthInfo{std::chrono::December}.end_weekday_no;

    //Elements row_item; much more apparent later
    std::vector<Elements> rows;
    for(int i=0; i<7; ++i){
        std::vector<Element> columns;
        for(int j=0; j<53; ++j){
            columns.push_back(text(" "));
            if ((j == 0 && i < jan1) || (j == 52 && i > dec31))
                columns.push_back(text(" "));
            else
                columns.push_back(text("") | color(Color::Red));   //nerd-font f0c8
        }
        rows.push_back(columns);
    }

    std::vector<Element> day;
    std::vector<std::string> days {"   ", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    for(auto x: days){
        day.push_back(text(x) | bold | color(Color::Red));
    }
    //day |= size(WIDTH, EQUAL, 3)

    std::vector<Element> space;
    for(int i=0; i<7; ++i)
        space.push_back(text(" "));

    std::vector<Element> month;
    //add a filler first element
    std::vector<std::string> months {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
        "Aug", "Sep", "Oct", "Nov", "Dec"};
    /*
    for(auto x: months){
        month.push_back(text(" ")); //to match the initial space of grid
        month.push_back(text(x) | bold | color(Color::Red));
        //depending upon the number of weeks a month has we put spaces
        // 4 for 4 week month 5 for 5 week.
        // for now 4 temporary value
        month.push_back(text("    "));
    }
    */

    for (size_t i=0; i<months.size(); ++i){
        month.push_back(text(" ")); //to match the initial space of grid
        month.push_back(text(months[i]) | bold | color(Color::Red));
        month.push_back(text("    "));      //4 spaces
        int weeks = weeksInMonth(choosen_year, std::chrono::month(i+1));
        if (weeks > 4){
            std::string space(weeks - 4, ' ');
            month.push_back(text(space));
        }
    }

    hbox(month);
    Element table =
        gridbox({
            {
                text(" ") | size(WIDTH, EQUAL, 4),
                hbox(month),
            },
            {
                vbox(day) | size(WIDTH, EQUAL, 4),
                gridbox(rows) | hscroll_indicator | xframe |
                size(WIDTH, ftxui::LESS_THAN, 45),
            }
        });


    auto renderer = Renderer([&]{
        return hbox({
            vbox(day) | size(WIDTH, EQUAL, 4),
            vbox({
                hbox(month),
                gridbox(rows),
            }) | hscroll_indicator | frame ,
        });
    });
    auto screen = App::TerminalOutput();
    screen.Loop(renderer);

    return 0;
}
