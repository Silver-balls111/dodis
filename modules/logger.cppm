module;
#include <cerrno>
export module betterman.logger;

import std;
import betterman.calender;

template <typename T>
concept filestream = requires(T &f) {
    { !f } -> std::convertible_to<bool>;
    f.is_open();
};

export struct Log {
    static void log(const std::string_view log_level,
                    const std::string_view msg,
                    bool write_to_cerr = false);
    static void log(bool write_to_cerr = false);
    static int fileOpenError(filestream auto &file,
                             const std::string_view file_name = " ",
                             bool write_to_cerr = true);
    inline static std::filesystem::path m_path;
};

void Log::log(const std::string_view log_level, const std::string_view msg, bool write_to_cerr) {
    std::ofstream log_file{m_path, std::ios::app};
    auto time = getZonedTime<false>();
    auto f_time = std::format("{:%Y-%m-%d %H:%M:%S}", time);
    auto entry = std::format("[{}] {}: {}\n", f_time, log_level, msg);
    log_file << entry;
    log_file.flush();
    if (write_to_cerr)
        std::cerr << entry << '\n';
}

void Log::log(bool write_to_cerr) {
    std::ofstream log_file{m_path, std::ios::app};
    auto time = getZonedTime<false>();
    auto f_time = std::format("{:%Y-%m-%d %H:%M:%S}", time);
    auto entry = std::format("[{}] ERROR: {}\n", f_time, std::strerror(errno));
    log_file << entry;
    log_file.flush();
    if (write_to_cerr)
        std::cerr << entry << '\n';
}

int Log::fileOpenError(filestream auto &file,
                       const std::string_view file_name,
                       bool write_to_cerr) {
    if (!file) {
        std::string err = std::format("{} {}", std::strerror(errno), file_name);
        Log::log("ERROR", err);
        if (write_to_cerr)
            std::cerr << "ERROR: " << err << '\n';
        return 1;
    }
    return 0;
}
// Log.log(ERROR, "file failed to open");
