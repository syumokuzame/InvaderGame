#include "Logger.h"
#include <filesystem>

namespace engine {

Logger& Logger::instance() {
    static Logger s;
    return s;
}

void Logger::init(const std::string& path) {
    // ディレクトリが存在しない場合は作成
    std::filesystem::path p(path);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }
    mFile.open(path, std::ios::trunc);
    log("[GAME] Start");
}

void Logger::log(const std::string& message) {
    if (mFile.is_open()) {
        mFile << message << "\n";
        mFile.flush();
    }
}

} // namespace engine
