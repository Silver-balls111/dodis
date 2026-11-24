#include <cstring>
#include <ftxui/ftxui.hpp>

import std;
import betterman.themeFile;
import betterman.colors;
import betterman.fileWatcher;
import betterman.logger;

namespace fs = std::filesystem;
fs::path base;
fs::path contributions_dir;
fs::path themes_dir;
fs::path task_fp;
fs::path completion_fp;

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

#ifdef _WIN32
    const char *home = std::getenv("APPDATA");
    if (!home) {
        std::cerr << "APPDATA not set\n";
        return 1;
    }
    base = fs::path(home) / "betterman";
#else
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cerr << "HOME not set\n";
        return 1;
    }
    base = fs::path(home) / ".config" / "betterman";
#endif
    if (!fs::exists(base))
        fs::create_directories(base);

    Log::m_path = base / "log.log";
    contributions_dir = base / "contributions";
    themes_dir = base / "themes";
    task_fp = base / "tasks.txt";
    completion_fp = base / "completion.txt";

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
