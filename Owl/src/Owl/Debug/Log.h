#pragma once
#include <fstream>
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
		 * \brief Outputs logging at the given level.
		 * \param pLevel The log level to use.
		 * \param pSender Name of the sender.
		 * \param pMessage The message to be logged.
		 * \param ... Any formatted data that should be included in the log entry.
		 */
		static void Print(LogLevel pLevel, const char* pSender, const char* pMessage, ...);

	private:
		static char* FormatMessage(const char* pFormat, ...);
		static char* FormatMessageV(const char* pFormat, va_list pVaList);
	};
}

// Core log macros

/**
 * \brief Logs a trace-level message. Should be used for verbose debugging purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_TRACE(pMessage, ...)    ::Owl::Log::Print(::Owl::LogLevel::Trace, "Owl", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs a debug-level message. Should be used for debugging purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_DEBUG(pMessage, ...)     ::Owl::Log::Print(::Owl::LogLevel::Debug, "Owl", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs an info-level message. Should be used for non-erroneous informational purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_INFO(pMessage, ...)     ::Owl::Log::Print(::Owl::LogLevel::Info, "Owl", pMessage, ##__VA_ARGS__)
/**
* \brief Logs a warning-level message. Should be used to indicate non-critical problems with
 * the application that cause it to run suboptimally.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_WARN(pMessage, ...)     ::Owl::Log::Print(::Owl::LogLevel::Warn, "Owl", pMessage, ##__VA_ARGS__)
/**
* \brief Logs an error-level message. Should be used to indicate critical runtime problems
 * that cause the application to run improperly or not at all.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_ERROR(pMessage, ...)    ::Owl::Log::Print(::Owl::LogLevel::Error, "Owl", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs a fatal-level message. Should be used to stop the application when hit.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CORE_CRITICAL(pMessage, ...) ::Owl::Log::Print(::Owl::LogLevel::Critical, "Owl", pMessage, ##__VA_ARGS__)

// Client log macros
/**
 * \brief Logs a debug-level message. Should be used for debugging purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_TRACE(pMessage, ...)    ::Owl::Log::Print(::Owl::LogLevel::Trace, "App", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs an info-level message. Should be used for non-erroneous informational purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_DEBUG_LOG(pMessage, ...)     ::Owl::Log::Print(::Owl::LogLevel::Debug, "App", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs an info-level message. Should be used for non-erroneous informational purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_INFO(pMessage, ...)     ::Owl::Log::Print(::Owl::LogLevel::Info, "App", pMessage, ##__VA_ARGS__)
/**
* \brief Logs a warning-level message. Should be used to indicate non-critical problems with
 * the application that cause it to run suboptimally.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_WARN(pMessage, ...)     ::Owl::Log::Print(::Owl::LogLevel::Warn, "App", pMessage, ##__VA_ARGS__)
/**
* \brief Logs an error-level message. Should be used to indicate critical runtime problems
 * that cause the application to run improperly or not at all.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_ERROR(pMessage, ...)    ::Owl::Log::Print(::Owl::LogLevel::Error, "App", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs a fatal-level message. Should be used to stop the application when hit.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define OWL_CRITICAL(pMessage, ...) ::Owl::Log::Print(::Owl::LogLevel::Critical, "App", pMessage, ##__VA_ARGS__)
