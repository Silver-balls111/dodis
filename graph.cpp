#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/ftxui.hpp>
#include <pthread.h>
#include <vector>

int main() {
    using namespace ftxui;

    Element document = frame(vbox({
        text("The window") | bold | color(Color::Blue),
        separator(),
        gauge(0.5),
        text("The footer"),
        text("The footer"),
        text("The footer"),
        text("The footer"),
    })
    );

    Element sep = separatorHSelector(20,5, Color::Red, Color::Yellow);
    Cell cell;
    cell.character = "X";
    document |= flex;
    document |= borderWith(cell);
    //didn't work, neither did frame
    document |= vscroll_indicator;

    // auto& cell = screen.CellAt(3, 3);
    // cell.character = "X";
    // cell.bold = true;
    // cell.foreground_color = Color::Red;

    // Element column = frame(
    //     vbox({
    //         text("▆") | color(Color::Red),
    //         text("▆") | color(Color::Yellow),
    //         text("▆") | color(Color::Blue),
    //         text("▆") | color(Color::Orange1),
    //     })
    // );

    unsigned int first_day = 4;
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

    // auto screen = Screen::Create(Dimension::Fit(table));
    // //Render(screen, document);
    // //Render(screen, sep);
    // Render(screen, table);
    // screen.Print();
    return 0;
}
