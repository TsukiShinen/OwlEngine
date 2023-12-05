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

		void Initialize(const WindowProps& pWindowProps);
		void Update() override;
		
		[[nodiscard]] uint32_t GetWidth() const override { return m_Data.Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Data.Height; }
		
		void SetEventCallback(const EventCallbackFn& pCallback) override { m_Data.EventCallback = pCallback; }
		
		void Sleep(const unsigned long pMilliseconds) override { ::Sleep(pMilliseconds); }

	private:
		LRESULT ProcessMessages(uint32_t pMessage, WPARAM pWParam, LPARAM pLParam);
		static LRESULT CALLBACK WindowProc(HWND pWindow, uint32_t pMessage, WPARAM pWParam, LPARAM pLParam);

		HINSTANCE m_Instance;
		HWND m_Window;

		struct WindowData
		{
			const char* Title;
			uint32_t Width , Height;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
