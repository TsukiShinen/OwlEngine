#pragma once

#include "Owl/Core/PlatformDetection.h"

#include <memory>

#ifdef OWL_DEBUG
#if defined(OWL_PLATFORM_WINDOWS)
#define OWL_DEBUGBREAK() __debugbreak()
#elif defined(OWL_PLATFORM_LINUX)
		#include <signal.h>
		#define OWL_DEBUGBREAK() raise(SIGTRAP)
#else
		#error "Platform doesn't support debugbreak yet!"
#endif
#define OWL_ENABLE_ASSERTS
#else
	#define OWL_DEBUGBREAK()
#endif

#define OWL_EXPAND_MACRO(x) x
#define OWL_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define OWL_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Owl
{
	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "Owl/Debug/Log.h"
#include "Owl/Debug/Assert.h"
