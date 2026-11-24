module;
#include <efsw/efsw.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/component/event.hpp>
export module betterman.fileWatcher;

import std;
import betterman.globals;
import betterman.themeFile;
import betterman.colors;
import betterman.logger;

export class FileHandler : public efsw::FileWatchListener {
  public:
    FileHandler(std::string theme_name_f, std::string theme_f)
        : theme_name_f(theme_name_f), theme_f(theme_f) {}
    void handleFileAction(efsw::WatchID, const std::string &dir,
                          const std::string &filename, efsw::Action action,
                          const std::string &oldfilename) override {
        if (filename == theme_f) {
            if (action == efsw::Actions::Modified) {
                if (auto *app = ftxui::App::Active())
                    app->PostEvent(ftxui::Event::Custom);
                else
                    Pallete::setTheme();
            }
            return;
        }
        if (filename == theme_name_f) {
            if (action == efsw::Actions::Modified) {
                ThemeFile tf;
                theme_f = tf.getActualThemeFilePath().filename();
                if (auto *app = ftxui::App::Active())
                    app->PostEvent(ftxui::Event::Custom);
                else
                    Pallete::setTheme();
            }
            if (action == efsw::Actions::Moved) {
                ThemeFile tf;
                theme_f = tf.getActualThemeFilePath().filename();
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

export void watchThemes() {
    // fh need to be static for fw to later use otherwise fw would try to access
    // fh and fh is no more so segfault, tf well why not,
    static ThemeFile tf;
    static FileHandler fh(tf.getPsuedoFilePath().filename().string(),
                          tf.getActualThemeFilePath().filename().string());

    // when fw goes out of scope the watcher thread is killed so it must be
    // static
    static efsw::FileWatcher fw;

    // Initialized once: a second explicit addWatch on the same directory fails
    // with FileRepeated, so the id is cached in the static.
    static const efsw::WatchID watch_id =
        fw.addWatch(themes_dir.string(), &fh, true);

    if (watch_id < 0) {
        Log::log("ERROR",
                 std::format("Failed to watch {}: {}", themes_dir.string(),
                             efsw::Errors::Log::getLastErrorLog()));
        return;
    }

    fw.watch();
}
