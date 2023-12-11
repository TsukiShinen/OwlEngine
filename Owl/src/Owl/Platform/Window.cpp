#include "opch.h"
#include "Window.h"

#ifdef OWL_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Owl
{
	Window* Window::Create(const WindowProps& pWindowProps)
	{
#ifdef OWL_PLATFORM_WINDOWS
		return new WindowsWindow(pWindowProps);
#else
		throw std::runtime_error("Can't find platform!");
		return nullptr;
#endif
	}
}
