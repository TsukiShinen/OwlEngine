#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdarg>

namespace Owl
{
	/**
	 * \brief Represents levels of logging
	 */
	enum LogLevel
	{
		Critical,
		Error,
		Warn,
		Info,
		Debug,
		Trace
	};

	class Log
	{
	public:
		/**
		 * \brief Initializes logging system. Call twice; once with state = 0 to get required memory size,
		 * then a second time passing allocated memory to state.
		 * \param pMemoryRequirement A pointer to hold the required memory size of internal state.
		 * \param pBlock 0 if just requesting memory requirement, otherwise allocated block of memory.
		 */
		static void Initialize(uint64_t* pMemoryRequirement, void* pBlock);
		/**
		 * \brief Shuts down the logging system.
		 */
		static void Shutdown();

		/**
		 * \brief Outputs logging at the given level.
		 * \param pLevel The log level to use.
		 * \param pSender Name of the sender.
		 * \param pMessage The message to be logged.
		 * \param ... Any formatted data that should be included in the log entry.
		 */
		void Print(LogLevel pLevel, const char* pSender, const char* pMessage, ...);

		static Log* Get() { return s_Log; }

	private:
		static char* FormatMessage(const char* pFormat, ...);
		static char* FormatMessageV(const char* pFormat, va_list pVaList);

		static Log* s_Log;
	};
}

// Core log macros

/**
 * \brief Logs a trace-level message. Should be used for verbose debugging purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_TRACE(pMessage, ...)    ::Owl::Log::Get()->Print(::Owl::LogLevel::Trace, "Owl", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs a debug-level message. Should be used for debugging purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_DEBUG(pMessage, ...)     ::Owl::Log::Get()->Print(::Owl::LogLevel::Debug, "Owl", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs an info-level message. Should be used for non-erroneous informational purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_INFO(pMessage, ...)     ::Owl::Log::Get()->Print(::Owl::LogLevel::Info, "Owl", pMessage, ##__VA_ARGS__)
/**
* \brief Logs a warning-level message. Should be used to indicate non-critical problems with
 * the application that cause it to run suboptimally.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_WARN(pMessage, ...)     ::Owl::Log::Get()->Print(::Owl::LogLevel::Warn, "Owl", pMessage, ##__VA_ARGS__)
/**
* \brief Logs an error-level message. Should be used to indicate critical runtime problems
 * that cause the application to run improperly or not at all.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_ERROR(pMessage, ...)    ::Owl::Log::Get()->Print(::Owl::LogLevel::Error, "Owl", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs a fatal-level message. Should be used to stop the application when hit.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_CRITICAL(pMessage, ...) ::Owl::Log::Get()->Print(::Owl::LogLevel::Critical, "Owl", pMessage, ##__VA_ARGS__)

// Client log macros
/**
 * \brief Logs a debug-level message. Should be used for debugging purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_TRACE(pMessage, ...)    ::Owl::Log::Get()->Print(::Owl::LogLevel::Trace, "App", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs an info-level message. Should be used for non-erroneous informational purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_DEBUG_LOG(pMessage, ...)     ::Owl::Log::Get()->Print(::Owl::LogLevel::Debug, "App", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs an info-level message. Should be used for non-erroneous informational purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_INFO(pMessage, ...)     ::Owl::Log::Get()->Print(::Owl::LogLevel::Info, "App", pMessage, ##__VA_ARGS__)
/**
* \brief Logs a warning-level message. Should be used to indicate non-critical problems with
 * the application that cause it to run suboptimally.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_WARN(pMessage, ...)     ::Owl::Log::Get()->Print(::Owl::LogLevel::Warn, "App", pMessage, ##__VA_ARGS__)
/**
* \brief Logs an error-level message. Should be used to indicate critical runtime problems
 * that cause the application to run improperly or not at all.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_ERROR(pMessage, ...)    ::Owl::Log::Get()->Print(::Owl::LogLevel::Error, "App", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs a fatal-level message. Should be used to stop the application when hit.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CRITICAL(pMessage, ...) ::Owl::Log::Get()->Print(::Owl::LogLevel::Critical, "App", pMessage, ##__VA_ARGS__)
