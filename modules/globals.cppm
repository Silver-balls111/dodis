export module betterman.globals;
import std;

namespace fs = std::filesystem;

export fs::path base;
export fs::path contributions_dir;
export fs::path themes_dir;
export fs::path task_fp;
export fs::path completion_fp;

export int initializePaths() {
#ifdef _WIN32
    const char *home = std::getenv("APPDATA");
    if (!home) {
        std::cerr << "%APPDATA% not set\n";
        return 1;
    }
    base = fs::path(home) / "betterman";
#else
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cerr << "$HOME not set\n";
        return 1;
    }
    base = fs::path(home) / ".config" / "betterman";
#endif

    if (!fs::exists(base))
        fs::create_directories(base);

    contributions_dir = base / "contributions";
    themes_dir = base / "themes";
    task_fp = base / "tasks.txt";
    completion_fp = base / "completion.txt";

    return 0;
}
