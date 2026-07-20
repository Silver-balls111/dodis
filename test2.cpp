#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

using namespace ftxui;

int main() {
  auto screen = ScreenInteractive::TerminalOutput();

  // --- state ---
  std::string name;
  int tab_index = 0;
  std::vector<std::string> tab_labels = {"Form", "About"};
  bool show_modal = false;

  // --- page 1: a form ---
  Component name_input = Input(&name, "Your name");
  Component submit_button = Button("Submit", [&] { show_modal = true; });
  Component form = Container::Vertical({name_input, submit_button});
  Component form_view = Renderer(form, [&] {
    return vbox({
        text("Enter your name:"),
        name_input->Render() | border,
        submit_button->Render(),
    });
  });

  // --- page 2: static info ---
  Component about_view = Renderer([] {
    return text("A tiny FTXUI demo.") | center;
  });

  // --- tab navigation menu + tab content ---
  Component tab_toggle = Toggle(&tab_labels, &tab_index);
  Component tab_content = Container::Tab({form_view, about_view}, &tab_index);

  Component main_view = Renderer(tab_content, [&] {
    return vbox({
        tab_toggle->Render(),
        separator(),
        tab_content->Render(),
    }) | border;
  });

  // --- modal dialog on top of everything ---
  Component dialog = Renderer([&] {
    return vbox({
               text("Hello, " + name + "!"),
               text("Press Enter to close."),
           }) |
           border | center;
  });
  Component dialog_wrapped = dialog | CatchEvent([&](Event e) {
    if (e == Event::Return) { show_modal = false; return true; }
    return false;
  });

  Component app = Container::Vertical({ main_view});
  app = Modal(app, dialog_wrapped, &show_modal);

  // --- global quit key ---
  app = app | CatchEvent([&](Event e) {
    if (e == Event::Character('q')) { screen.Exit(); return true; }
    return false;
  });

  screen.Loop(app);
}
