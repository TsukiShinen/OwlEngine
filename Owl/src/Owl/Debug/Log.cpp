#include "opch.h"
#include "Owl/Platform/Window.h"

#include <ctime>

#include "Owl/Platform/FilesSystem.h"
#include "Platform/Windows/WindowsWindow.h"

namespace Owl
{
	Log* Log::s_Instance;
	
	void Log::Initialize()
	{
		OWL_CORE_ASSERT(!s_Instance, "Can only have one instance of Log")

		s_Instance = new Log();
		s_Instance->m_LogFile = new File();

		if (!FilesSystem::TryOpen("console.log", FileModeWrite | FileModeNew, false, s_Instance->m_LogFile))
			Window::ConsoleWriteError("[Log] Unable to open console.log for writing.\n", Error);
	}

	void Log::Shutdown()
	{
		if (s_Instance->m_LogFile)
			FilesSystem::Close(s_Instance->m_LogFile);
		delete s_Instance->m_LogFile;
		delete s_Instance;
	}

	void Log::Print(const LogLevel pLevel, const char* pSender, const char* pMessage, ...)
	{
		const char* levelStrings[6] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};
		const std::time_t now = std::time(nullptr);
		const std::tm* timeInfo = std::localtime(&now);
		char timestamp[9];
		std::strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeInfo);

		const bool isError = pLevel < Warn;
		
		va_list args;
		va_start(args, pMessage);
		auto message = FormatMessageV(pMessage, args);
		va_end(args);

		message = LogFormatMessage("[%s] [%s] %s: %s\n", timestamp, levelStrings[pLevel], pSender, message);

		if (isError)
			Window::ConsoleWriteError(message, pLevel);
		else
			Window::ConsoleWrite(message, pLevel);

		AppendToLogFile(message);

		delete[] message;
	}

	char* Log::LogFormatMessage(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		const auto buffer = FormatMessageV(pFormat, args);
		va_end(args);

		return buffer;
	}

	char* Log::FormatMessageV(const char* pFormat, va_list pVaList)
	{
		// Calculate the size needed for the formatted string
		const int size = std::vsnprintf(nullptr, 0, pFormat, pVaList);
		if (size <= 0)
		{
			va_end(pVaList);
			return nullptr;
		}

		// Allocate a buffer and format the string
		const auto buffer = new char[size + 1];
		std::vsnprintf(buffer, size + 1, pFormat, pVaList);
		return buffer;
	}

	void Log::AppendToLogFile(const char* pMessage)
	{
		if (!m_LogFile->IsValid)
			return;

		uint64_t length = strlen(pMessage);
		uint64_t written = 0;
		if (!FilesSystem::TryWrite(m_LogFile, length, pMessage, &written))
			Window::ConsoleWriteError("[Log] Error writing to console.log.", Error);
	}
}
