#pragma once

#include "Owl/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Owl
{
	class Log
	{
	public:
		static void Initialize(uint64_t* pMemoryRequirement, void* pBlock);
		static void Shutdown();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_Log->m_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_Log->m_ClientLogger; }

	private:
		Ref<spdlog::logger> m_CoreLogger;
		Ref<spdlog::logger> m_ClientLogger;

		static Log* s_Log;
	};
}

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

// Core log macros
#define OWL_CORE_TRACE(...)    ::Owl::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define OWL_CORE_INFO(...)     ::Owl::Log::GetCoreLogger()->info(__VA_ARGS__)
#define OWL_CORE_WARN(...)     ::Owl::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define OWL_CORE_ERROR(...)    ::Owl::Log::GetCoreLogger()->error(__VA_ARGS__)
#define OWL_CORE_CRITICAL(...) ::Owl::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define OWL_TRACE(...)         ::Owl::Log::GetClientLogger()->trace(__VA_ARGS__)
#define OWL_INFO(...)          ::Owl::Log::GetClientLogger()->info(__VA_ARGS__)
#define OWL_WARN(...)          ::Owl::Log::GetClientLogger()->warn(__VA_ARGS__)
#define OWL_ERROR(...)         ::Owl::Log::GetClientLogger()->error(__VA_ARGS__)
#define OWL_CRITICAL(...)      ::Owl::Log::GetClientLogger()->critical(__VA_ARGS__)
