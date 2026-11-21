#include "helpers/colors.hpp"
#include "helpers/globals.hpp"
#include <cstring>
#include <efsw/efsw.hpp>
#include <filesystem>
#include <ftxui/ftxui.hpp>
#include <iostream>
#include <string>

void Create();
void Update();
void Graph(bool interactive);
void Help();
void Edit();
void themeSwitcher();

namespace fs = std::filesystem;

fs::path getThemeFilePath() {
    ThemeFile tf;
    return tf.getActualThemeFilePath();
}

class FileHandler : public efsw::FileWatchListener {
  public:
    FileHandler(std::string theme_name_f, std::string theme_f)
        : theme_name_f(theme_name_f), theme_f(theme_f) {}
    void handleFileAction(efsw::WatchID, const std::string &dir,
                          const std::string &filename, efsw::Action action,
                          const std::string &oldfilename) override {
        if (filename == theme_f) {
            if (action == efsw::Actions::Modified) {
                // requestThemeReload();
                if (auto *app = ftxui::App::Active())
                    app->PostEvent(ftxui::Event::Custom);
                else
                    Pallete::setTheme();
            }
            return;
        }
        if (filename == theme_name_f) {
            if (action == efsw::Actions::Modified) {
                theme_f = getThemeFilePath().filename();
                // requestThemeReload();
                if (auto *app = ftxui::App::Active())
                    app->PostEvent(ftxui::Event::Custom);
                else
                    Pallete::setTheme();
            }
            return;
        }
        return;
    }

  private:
    std::string theme_name_f;
    std::string theme_f;
};

int main(const int argc, const char **argkv) {
    if (argc == 1) {
        fs::path path{"tasks.txt"};
        if (!std::filesystem::exists(path) || std::filesystem::is_empty(path))
            Create();
        else
            Update();
    }

    ThemeFile tf;
    auto pfp = tf.getPsuedoFilePath();
    fs::path afp = tf.getActualThemeFilePath();
    FileHandler fh(pfp.filename(), afp.filename());

    efsw::FileWatcher fw;
    fw.addWatch("themes/", &fh, true);
    fw.watch();

    if (argc == 2) {
        if (!strcmp(argkv[1], "-c")) { // strcmp return 0 if equal so !
            Create();
            return 0;
        } else if (!strcmp(argkv[1], "-u")) {
            Update();
            // NOTE: not whether to use this or altscreen.
            // std::cout << "\033[2J\033[H" << std::flush; // clear screen,
            // cursor to top-left
            Graph(false);
            return 0;
        } else if (!strcmp(argkv[1], "-g")) {
            Graph(true);
            return 0;
        } else if (!strcmp(argkv[1], "-e")) {
            Edit();
            return 0;
        } else if (!strcmp(argkv[1], "-t")) {
            themeSwitcher();
            return 0;
        } else if (!strcmp(argkv[1], "-h")) {
            Help();
            return 0;
        } else {
            std::cerr << "Invalid argument.";
            Help();
            return 1;
        }
    } else {
        std::cerr << "Invalid argument.";
        Help();
        return 1;
    }
}
