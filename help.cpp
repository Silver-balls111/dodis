#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

void Help(){
    using namespace ftxui;

    auto help = vbox({
        text("Usage: ./dodis [argument]"),
        hbox({
            vbox({
                text("Arguments"),
                text("-c"),
                text("-u"),
                text("-g"),
                text("-h"),
            }),
            text("      "),
            vbox({
                text("Description"),
                text("Create new list"),
                text("Update contributions"),
                text("Show contribution graph"),
                text("Show this menu"),
            })
        })
    });

    auto screen = Screen::Create(Dimension::Fit(help));
    Render(screen, help);
    screen.Print();
}
