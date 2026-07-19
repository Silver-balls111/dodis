#include <chrono>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
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

    unsigned int jan1 = MonthInfo{choosen_year, std::chrono::January}.start_weekday_no;
    unsigned int dec31 = MonthInfo{choosen_year, std::chrono::December}.end_weekday_no;

    // One tiny invisible focusable "cursor" per calendar week-column.
    // The `bool focused` overload of Renderer makes each one Focusable()
    // automatically -- no manual override needed. Putting them all in a
    // Container::Horizontal gives us Left/Right navigation between them
    // for free (built into FTXUI), so no manual key-handling or clamping.
    std::vector<Component> cursors;
    for (int j = 0; j < 53; ++j){
        cursors.push_back(Renderer([](bool focused) -> Element {
            return focused ? focus(text("")) : text("");
        }));
    }
    auto cursor_row = Container::Horizontal(cursors);

    // Builds the grid fresh each frame. Each top-row cell gets its cursor's
    // rendered (invisible) element overlaid via dbox, so the cursor sits at
    // exactly the same on-screen x-position as its column. When that cursor
    // is the focused one, xframe (below) auto-scrolls to reveal it.
    auto build_rows = [&]{
        std::vector<Elements> rows;
        for(int i=0; i<7; ++i){
            std::vector<Element> columns;
            for(int j=0; j<53; ++j){
                columns.push_back(text(" "));

                Element cell;
                if ((j == 0 && i < jan1) || (j == 52 && i > dec31))
                    cell = text(" ");
                else
                    cell = text("") | color(Color::Red);   //nerd-font f0c8

                if (i == 0)
                    cell = dbox({cell, cursors[j]->Render()});

                columns.push_back(cell);
            }
            rows.push_back(columns);
        }
        return rows;
    };

    std::vector<Element> day;
    std::vector<std::string> days {"   ", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; //initial space it will be a vbox item
    for(auto x: days){
        day.push_back(text(x) | bold | color(Color::Red));
    }

    std::vector<Element> month;
    std::vector<std::string> months {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
        "Aug", "Sep", "Oct", "Nov", "Dec"};

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

    // hbox(month);
    // Element table =
    //     gridbox({
    //         {
    //             text(" ") | size(WIDTH, EQUAL, 4),
    //             hbox(month),
    //         },
    //         {
    //             vbox(day) | size(WIDTH, EQUAL, 4),
    //             gridbox(rows) | hscroll_indicator | xframe |
    //             size(WIDTH, ftxui::LESS_THAN, 45),
    //         }
    //     });

    // h2 wraps cursor_row as its child component so key events (Left/Right)
    // reach the cursors' built-in navigation.
    auto h2 = Renderer(cursor_row, [&]{
        return vbox({
            hbox(month),
            gridbox(build_rows()),
        });
    });

    auto h1 = Renderer([&]{
        return vbox(day) ;
    });

    auto rend = Renderer(h2, [&]{
        return hbox({
            h1->Render() | size(WIDTH, ftxui::EQUAL, 4),
            h2->Render() | hscroll_indicator | xframe
        }) | border;
    });

    auto screen = App::TerminalOutput();
    screen.Loop(rend);

    return 0;
}
