#include <ftxui/dom/elements.hpp>
#include <ftxui/ftxui.hpp>

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

    //Elements row_item; much more apparent later
    std::vector<Element> row_item;
    for(int i=0; i<7; ++i){
        row_item.push_back(text("▆") | color(Color::Red));
    }

    std::vector<Elements> colum_item;
    for(int i=0; i<53; ++i){
        colum_item.push_back(row_item);
    }

    std::vector<Elements> rows;
    for(int i=0; i<7; ++i){
        std::vector<Element> columns;
        for(int j=0; j<53; ++j){
            columns.push_back(text("▆") | color(Color::Red));   //lower three quarter block
            columns.push_back(text(" "));
        }
        rows.push_back(columns);
    }

    Element table = frame(
        gridbox(rows)
    );

    auto screen = Screen::Create(Dimension::Fit(table));
    //Render(screen, document);
    //Render(screen, sep);
    Render(screen, table);
    screen.Print();
    return 0;
}
