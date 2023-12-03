#include "opch.h"
#include "Platform.h"

#include "Platform/Windows.h"

namespace Owl
{
	Platform* Platform::Create(const WindowProps& pWindowProps)
	{
#ifdef OWL_PLATFORM_WINDOWS
		return new Windows(pWindowProps);
#else
		throw std::runtime_error("Can't find platform!");
		return nullptr;
#endif
	}
}
