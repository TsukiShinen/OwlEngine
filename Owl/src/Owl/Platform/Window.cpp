#include "opch.h"
#include "Window.h"

#ifdef OWL_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Owl
{
	Scope<Window> Window::Create(const WindowProps& pWindowProps)
	{
#ifdef OWL_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(pWindowProps);
#else
		throw std::runtime_error("Can't find platform!");
		return nullptr;
#endif
	}
}
