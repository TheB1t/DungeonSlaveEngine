#include "log.hpp"

Log::Log(std::string name) : name(name) {
	AllocConsole();
}

Log::Log(std::string name, std::string customInfo) : name(name), customInfo("[" + customInfo + "]") {
	AllocConsole();
}

std::string Log::levelToString(Log::Level level) {
	switch (level) {
		case Log::Level::INFO: return "INFO";
		case Log::Level::WARN: return "WARN";
		case Log::Level::ERR:  return "ERR";
		case Log::Level::FATAL: return "FATAL";
		case Log::Level::DEBUG: return "DEBUG";
	}

	return "UNKN";
}

std::string Log::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm timeInfo;
    localtime_s(&timeInfo, &currentTime);

    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
