#include <chrono>
#include <ftxui/ftxui.hpp>

struct MonthInfo{

};
unsigned int weekday_no(std::chrono::year_month_day ymd){
    std::chrono::sys_days sd = ymd;
    return std::chrono::weekday(sd).c_encoding();
}

int main(){
    using namespace ftxui;

    std::chrono::year_month_day today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    unsigned int first_day = weekday_no(today.year(), std::chrono::January, 1);
    unsigned int last_day = 4;

    //Elements row_item; much more apparent later
    std::vector<Elements> rows;
    for(int i=0; i<7; ++i){
        std::vector<Element> columns;
        for(int j=0; j<53; ++j){
            columns.push_back(text(" "));
            if (j == 0 && i < first_day)
                columns.push_back(text(" "));
            else if (j == 52 && i > last_day)
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
    for(auto x: months){
        month.push_back(text(" ")); //to match the initial space of grid
        month.push_back(text(x) | bold | color(Color::Red));
        //depending upon the number of weeks a month has we put spaces
        // 4 for 4 week month 5 for 5 week.
        // for now 4 temporary value
        month.push_back(text("    "));
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
