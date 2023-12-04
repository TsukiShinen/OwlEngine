#include "Windows.h"

#include <stdexcept>
#include <windowsx.h>

#include "Owl/Debug/Instrumentor.h"

namespace Owl
{
	Windows::Windows(const WindowProps& pWindowProps)
		: m_WindowProps(pWindowProps)
	{
		OWL_PROFILE_FUNCTION();
		m_HInstance = GetModuleHandleA(nullptr);

		WNDCLASSA windowClass = {};
		windowClass.style = CS_DBLCLKS;
		windowClass.lpfnWndProc = ProcessMessages;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = m_HInstance;
		windowClass.hIcon = LoadIcon(m_HInstance, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = nullptr;
		windowClass.lpszClassName = "owl_window_class";

		if (!RegisterClassA(&windowClass))
		{
			throw std::runtime_error("[PlatformWindows] Failed to register window!");
		}

		uint32_t windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
		uint32_t windowExStyle = WS_EX_APPWINDOW;

		windowStyle |= WS_MAXIMIZE;
		windowStyle |= WS_MINIMIZE;
		windowStyle |= WS_THICKFRAME;

		RECT borderRect = {0, 0, 0, 0};
		AdjustWindowRectEx(&borderRect, windowStyle, 0, windowExStyle);

		const uint32_t windowX = pWindowProps.X + borderRect.left;
		const uint32_t windowY = pWindowProps.Y + borderRect.top;
		const uint32_t windowWidth = pWindowProps.Width + borderRect.right - borderRect.left;
		const uint32_t windowHeight = pWindowProps.Height + borderRect.bottom - borderRect.left;

		m_HWindow = CreateWindowExA(windowExStyle, "owl_window_class", m_WindowProps.ApplicationName, windowStyle,
		                            windowX, windowY, windowWidth, windowHeight,
		                            nullptr, nullptr, m_HInstance, nullptr);

		if (m_HWindow == nullptr)
			throw std::runtime_error("[PlatformWindows] Failed to create window!");

		ShowWindow(m_HWindow, SW_SHOW);
	}

	Windows::~Windows()
	{
		OWL_PROFILE_FUNCTION();
		if (m_HWindow)
		{
			DestroyWindow(m_HWindow);
			m_HWindow = nullptr;
		}
	}

	bool Windows::PumpMessages()
	{
		OWL_PROFILE_FUNCTION();
		MSG message;
		while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
		{
			OWL_PROFILE_SCOPE("Process Message");
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}

		return true;
	}

	LRESULT Windows::ProcessMessages(const HWND pHWindow, const uint32_t pMessage, const WPARAM pWParam,
	                                 const LPARAM pLParam)
	{
		OWL_PROFILE_FUNCTION();
		switch (pMessage)
		{
		case WM_ERASEBKGND:
			return 1;
		case WM_CLOSE:
			// TODO: Quit Application
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SIZE:
			{
				RECT r;
				GetClientRect(pHWindow, &r);
				uint32_t width = r.right - r.left;
				uint32_t height = r.bottom - r.top;

				// TODO: Event for Window resize
			}
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			{
				bool isPressed = pMessage == WM_KEYDOWN || pMessage == WM_SYSKEYDOWN;
				// TODO: Input processing
			}
			break;
		case WM_MOUSEMOVE:
			{
				int x = GET_X_LPARAM(pLParam);
				int y = GET_Y_LPARAM(pLParam);
				// TODO: Input processing
			}
			break;
		case WM_MOUSEWHEEL:
			{
				int zDelta = GET_WHEEL_DELTA_WPARAM(pWParam);
				if (zDelta != 0)
					zDelta = zDelta < 0 ? -1 : 1;
				// TODO: Input processing
			}
			break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			{
				bool isPressed = pMessage == WM_KEYDOWN || pMessage == WM_SYSKEYDOWN;
				// TODO: Input processing
			}
			break;
		}

		return DefWindowProcA(pHWindow, pMessage, pWParam, pLParam);
	}
}
