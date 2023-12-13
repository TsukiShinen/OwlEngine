#pragma once

#include "Owl/Core/Base.h"
#include <filesystem>

#ifdef OWL_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define OWL_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { OWL##type##ERROR(msg, __VA_ARGS__); OWL_DEBUGBREAK(); } }
#define OWL_INTERNAL_ASSERT_WITH_MSG(type, check, ...) OWL_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: %lld", __VA_ARGS__)
#define OWL_INTERNAL_ASSERT_NO_MSG(type, check) OWL_INTERNAL_ASSERT_IMPL(type, check, "Assertion '%s' failed at %s:%s", OWL_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define OWL_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define OWL_INTERNAL_ASSERT_GET_MACRO(...) OWL_EXPAND_MACRO( OWL_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, OWL_INTERNAL_ASSERT_WITH_MSG, OWL_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define OWL_ASSERT(...) OWL_EXPAND_MACRO( OWL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define OWL_CORE_ASSERT(...) OWL_EXPAND_MACRO( OWL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define OWL_ASSERT(...)
	#define OWL_CORE_ASSERT(...)
#endif
