#pragma once

#include "Owl/Events/Event.h"

namespace Owl
{
	struct WindowProps
	{
		std::string Title = "Owl Application";
		uint32_t Width = 1600;
		uint32_t Height = 900;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		
		virtual ~Window() = default;

		virtual void Update() = 0;

		[[nodiscard]] virtual uint32_t GetWidth() const = 0;
		[[nodiscard]] virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& pCallback) = 0;
		
		virtual void Sleep(unsigned long pMilliseconds) = 0;

		virtual const char* GetVulkanRequiredExtension() = 0;
		
		static Scope<Window> Create(const WindowProps& pWindowProps = WindowProps());
	};
}
