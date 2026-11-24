module;
#include <efsw/efsw.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/app.hpp>
export module betterman.fileWatcher;

import std;
import betterman.themeFile;
import betterman.colors;

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

export void watchThemes(){
    ThemeFile tf;
    auto pfp = tf.getPsuedoFilePath();
    std::filesystem::path afp = tf.getActualThemeFilePath();
    FileHandler fh(pfp.filename(), afp.filename());

    efsw::FileWatcher fw;
    fw.addWatch(pfp.parent_path().string(), &fh, true);
    fw.watch();
}
