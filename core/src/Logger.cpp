#include "StdInclude.hpp"
#include "Logger.hpp"

namespace IWXMVM
{
	constexpr auto LOGGER_NAME = "IWXMVM";
	constexpr auto LOG_FILE = "IWXMVM.log";

	std::shared_ptr<spdlog::logger> Logger::internalLogger;

	void Logger::Initialize()
	{
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>(spdlog::color_mode::always));
		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(LOG_FILE, (size_t)5e6, 1));
		internalLogger = std::make_shared<spdlog::logger>(LOGGER_NAME, sinks.begin(), sinks.end());
		internalLogger->set_pattern("[%d.%m.%C %H:%M:%S] [%n] [%^%l%$] %v");
		internalLogger->flush_on(spdlog::level::trace);
		internalLogger->set_level(spdlog::level::debug);
		LOG_INFO("Initialized Logger");
	}

	std::shared_ptr<spdlog::logger> Logger::GetInternalLogger()
	{
		return internalLogger;
	}
}