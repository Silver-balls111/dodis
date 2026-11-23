module;
#include <cerrno>
export module betterman.themeFile;
import std;

export class ThemeFile {
    inline static std::mutex psuedo_file_mtx_;
    inline static std::mutex actual_file_mtx_;
    std::filesystem::path psuedo_file_path_{"themes/theme.txt"};
    std::filesystem::path actual_file_path_;

  public:
    std::expected<int, std::string_view> writePsuedoFile(std::string &content) {
        std::scoped_lock lock(psuedo_file_mtx_);
        try {
            // ofstream dosne't throw if file can't be opened the << will throw
            std::ofstream f{psuedo_file_path_};
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
        }
        return actual_file_path_;
    }
    auto writeActualThemeFile() {
        std::scoped_lock lock(actual_file_mtx_);
        return std::ofstream{getActualThemeFilePath()};
    }
    auto readActualThemeFile() {
        std::scoped_lock lock(actual_file_mtx_);
        return std::ifstream{getActualThemeFilePath()};
    }
};
