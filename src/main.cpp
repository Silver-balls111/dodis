#include <cstring>
#include <ftxui/ftxui.hpp>

import std;
import betterman.themeFile;
import betterman.colors;
import betterman.fileWatcher;
import betterman.logger;
import betterman.globals;

namespace fs = std::filesystem;

// this fun are not exported from a module as if the file changes then bmi
// gets build again taking more build time than reduce.
void Create();
void Update();
void Graph(bool interactive);
void Help();
void Edit();
void themeSwitcher();

int main(const int argc, const char **argkv) {
    Pallete::setTheme();
    if (initializePaths())
        return 1;

    Log::m_path = base / "log.log";

    if (argc == 1) {
        if (!fs::exists(task_fp) || fs::is_empty(task_fp))
            Create();
        else
            Update();
    }
    watchThemes();
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
