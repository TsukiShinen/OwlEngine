#include "opch.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Owl
{
	Log* Log::s_Log = nullptr;

	void Log::Initialize(uint64_t* pMemoryRequirement, void* pBlock)
	{
		OWL_PROFILE_FUNCTION();
		*pMemoryRequirement = sizeof(Log);
		if (pBlock == nullptr)
			return;
		s_Log = static_cast<Log*>(pBlock);
		
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("OWL.log", true));

		logSinks[0]->set_pattern("%^[%T] [%l] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_Log->m_CoreLogger = std::make_shared<spdlog::logger>("OWL", begin(logSinks), end(logSinks));
		register_logger(s_Log->m_CoreLogger);
		s_Log->m_CoreLogger->set_level(spdlog::level::trace);
		s_Log->m_CoreLogger->flush_on(spdlog::level::trace);

		s_Log->m_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		register_logger(s_Log->m_ClientLogger);
		s_Log->m_ClientLogger->set_level(spdlog::level::trace);
		s_Log->m_ClientLogger->flush_on(spdlog::level::trace);
	}

	void Log::Shutdown()
	{
		OWL_PROFILE_FUNCTION();
		s_Log = nullptr;
	}
}
