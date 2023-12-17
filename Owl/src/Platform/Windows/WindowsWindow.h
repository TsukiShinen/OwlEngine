#pragma once

#include <cstdint>
#include <windows.h>

#include "Owl/Platform/Window.h"

namespace Owl
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& pWindowProps);
		~WindowsWindow() override;

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagPlatform); }

		void operator delete(void* pBlock, const size_t pSize)
		{
			return OWL_FREE(pBlock, pSize, Owl::MemoryTagPlatform);
		}

		void Initialize(const WindowProps& pWindowProps);
		VkSurfaceKHR CreateVulkanSurface(const VulkanContext* pRenderer) override;
		void Update() override;

		[[nodiscard]] uint32_t GetWidth() const override { return m_Data.Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Data.Height; }

		void SetEventCallback(const EventCallbackFn& pCallback) override { m_Data.EventCallback = pCallback; }

		void Sleep(const unsigned long pMilliseconds) override { ::Sleep(pMilliseconds); }

		const char* GetVulkanRequiredExtension() override { return "VK_KHR_win32_surface"; }

	private:
		LRESULT ProcessMessages(uint32_t pMessage, WPARAM pWParam, LPARAM pLParam);
		static LRESULT CALLBACK WindowProc(HWND pWindow, uint32_t pMessage, WPARAM pWParam, LPARAM pLParam);

		HINSTANCE m_Instance;
		HWND m_Window;

		CONSOLE_SCREEN_BUFFER_INFO m_StdOutputCsbi;
		CONSOLE_SCREEN_BUFFER_INFO m_ErrOutputCsbi;

		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		friend class Window;
	};
}
