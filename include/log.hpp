#pragma once

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <format>
#include <chrono>
#include <ctime>
#include <iomanip>

class Log {
	public:
		typedef enum {
			INFO		= 0,
			WARN		= 1,
			ERR			= 2,
			FATAL		= 3,
			DEBUG		= 4,
		} Level;

		class Assert {
			public:
				bool cond = false;
				
				Assert(bool cond) : cond(cond) {};

				template<typename T>
				friend Assert& operator<<(Assert& assert, T& msg) {
					if (assert.cond)
						std::cout << msg;

					return assert;
				}

				friend Assert& operator<<(Assert& assert, std::ostream& (*manip)(std::ostream&)) {
					if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl) && assert.cond) {
						std::cout << std::endl;
						PostQuitMessage(0);
					}
					return assert;
				}
		};

		static const Log::Level globalLogLevel = Log::Level::DEBUG;

		Log(std::string name);
		Log(std::string name, std::string customInfo);

		friend Log& operator<<(Log& log, Log::Level level) {
			log.msgBlocked = level > Log::globalLogLevel ? true : false;

			if (!log.msgBlocked) log << std::format("[{}][{}]{}[{}] ", log.getCurrentTime(), log.name, log.customInfo, log.levelToString(level));
			return log;
		}

		friend Assert& operator<<(Log& log, Log::Assert assert) {
			if (assert.cond)
				log << Log::Level::FATAL << "[Assertion Failed] ";

			return assert;
		}

		friend Log& operator<<(Log& log, std::ostream& (*manip)(std::ostream&)) {
			if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) {
				std::cout << std::endl;
			}
			return log;
		}

		template<typename T>
		friend Log& operator<<(Log& log, T& msg) {
			if (!log.msgBlocked) std::cout << msg;
			return log;
		}
	
		template<typename T>
		Log& operator<<(const T& msg) {
			if (!this->msgBlocked) std::cout << msg;
			return *this;
		}

		std::string levelToString(Log::Level level);
		std::string getCurrentTime();

	private:
		std::string name = "Unknown";
		std::string customInfo = "";

		bool msgBlocked = false;
};
