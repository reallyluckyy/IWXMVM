#pragma once

#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#pragma warning(pop)

#define LOG_INFO(...) Logger::GetInternalLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) Logger::GetInternalLogger()->warn(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::GetInternalLogger()->debug(__VA_ARGS__)
#define LOG_ERROR(...) Logger::GetInternalLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::GetInternalLogger()->critical(__VA_ARGS__)

namespace IWXMVM
{

	class Logger
	{
	public:
		static void Initialize();
		static std::shared_ptr<spdlog::logger> GetInternalLogger();
	private:
		static std::shared_ptr<spdlog::logger> internalLogger;
	};
}