module;
#include <cerrno>
export module betterman.themeFile;

import std;
import betterman.globals;
import betterman.logger;

namespace fs = std::filesystem;

export class ThemeFile {
  public:
    std::expected<int, std::string_view> writePsuedoFile(std::string &content) {
        std::scoped_lock lock(psuedo_file_mtx_);
        try {
            // ofstream dosne't throw if file can't be opened the << will throw
            std::ofstream f{psuedo_file_path_};
            if (Log::fileOpenError(f, psuedo_file_path_.string(), false))
                throw std::runtime_error("");
            f << content;
            f.flush();
            return 0;
        } catch (...) {
            return std::unexpected(std::strerror(errno));
        }
    }
    std::expected<std::string, std::string> readPsuedoFile() {
        std::scoped_lock lock(psuedo_file_mtx_);
        try {
            std::ifstream f{psuedo_file_path_};
            if (Log::fileOpenError(f, psuedo_file_path_.string(), false))
                throw std::runtime_error("");
            std::stringstream s;
            s << f.rdbuf();
            return std::move(s.str());
        } catch (...) {
            return std::unexpected(std::strerror(errno));
        }
    }
    auto getPsuedoFilePath() const { return psuedo_file_path_; }
    auto getActualThemeFilePath() {
        auto name{readPsuedoFile()};
        if (name.has_value()) {
            actual_file_path_ = name.value();
            return actual_file_path_;
        }
        else
            return fs::path {themes_dir / "Default.json"};
    }

  private:
    inline static std::mutex psuedo_file_mtx_; // current_theme.txt file
    fs::path psuedo_file_path_{theme_fp};
    fs::path actual_file_path_;
};
