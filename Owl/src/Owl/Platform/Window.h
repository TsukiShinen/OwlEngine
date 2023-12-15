#pragma once

#include <vulkan/vulkan.h>

#include "Owl/Events/Event.h"
#include "Renderer/Vulkan/VulkanRendererApi.h"

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
		
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagPlatform); }
		void operator delete(void* pBlock, const size_t pSize) { return OWL_FREE(pBlock, pSize, Owl::MemoryTagPlatform); }

		virtual ~Window() = default;

		virtual VkSurfaceKHR CreateVulkanSurface(const VulkanContext* pRenderer) = 0;
		virtual void Update() = 0;

		[[nodiscard]] virtual uint32_t GetWidth() const = 0;
		[[nodiscard]] virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& pCallback) = 0;

		virtual void Sleep(unsigned long pMilliseconds) = 0;

		static void ConsoleWrite(const char* pMessage, uint8_t pColour);
		static void ConsoleWriteError(const char* pMessage, uint8_t pColour);

		virtual const char* GetVulkanRequiredExtension() = 0;

		static Window* Create(const WindowProps& pWindowProps = WindowProps());
	};
}
