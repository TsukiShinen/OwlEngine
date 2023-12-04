#include "opch.h"
#include "Platform.h"

#include "Platform/Windows.h"

namespace Owl
{
	Platform* Platform::s_Instance;

	void Platform::Initialize(const WindowProps& pWindowProps)
	{
#ifdef OWL_PLATFORM_WINDOWS
		s_Instance = new Windows(pWindowProps);
#else
		throw std::runtime_error("Can't find platform!");
#endif
	}

	void Platform::Shutdown()
	{
		delete s_Instance;
	}
}
