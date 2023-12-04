#pragma once

#include <cstdint>
#include <windows.h>

#include "Owl/Platform/Platform.h"

namespace Owl
{
	class Windows : public Platform
	{
	public:
		Windows(const WindowProps& pWindowProps);
		~Windows();

		Windows(const Windows&) = delete;
		Windows& operator=(const Windows&) = delete;

	private:
		bool PumpMessages() override;

		void Sleep(const unsigned long pMilliseconds) override { ::Sleep(pMilliseconds); }

		static LRESULT CALLBACK ProcessMessages(HWND pHWindow, uint32_t pMessage, WPARAM pWParam, LPARAM pLParam);

		HINSTANCE m_HInstance;
		HWND m_HWindow;

		WindowProps m_WindowProps;
	};
}
