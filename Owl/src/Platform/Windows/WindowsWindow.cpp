#include "WindowsWindow.h"

#include <stdexcept>
#include <windowsx.h>

#include "Owl/Debug/Instrumentor.h"
#include "Owl/Events/ApplicationEvent.h"
#include "Owl/Events/KeyEvent.h"
#include "Owl/Events/MouseEvent.h"
#include "Owl/Platform/Input.h"

namespace Owl
{
	static uint8_t s_WindowCount = 0;
	
	WindowsWindow::WindowsWindow(const WindowProps& pWindowProps)
	{
		OWL_PROFILE_FUNCTION();

		Initialize(pWindowProps);
	}

	WindowsWindow::~WindowsWindow()
	{
		OWL_PROFILE_FUNCTION();
		if (m_Window)
		{
			DestroyWindow(m_Window);
			m_Window = nullptr;
		}
		s_WindowCount--;
	}

	void WindowsWindow::Initialize(const WindowProps& pWindowProps)
	{
		OWL_PROFILE_FUNCTION();
		
		m_Data.Title = pWindowProps.Title;
		m_Data.Width = pWindowProps.Width;
		m_Data.Height = pWindowProps.Height;

		OWL_CORE_INFO("Creating Window {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);

		m_Instance = GetModuleHandleA(nullptr);
		OWL_CORE_ASSERT(m_Instance, "[WindowsWindow] Failed to create window instance!")

		WNDCLASSA windowClass = {};
		windowClass.style = CS_DBLCLKS;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = m_Instance;
		windowClass.hIcon = LoadIcon(m_Instance, IDI_APPLICATION);
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

		const uint32_t windowX = 100 + borderRect.left;
		const uint32_t windowY = 100 + borderRect.top;
		const uint32_t windowWidth = m_Data.Width + borderRect.right - borderRect.left;
		const uint32_t windowHeight = m_Data.Height + borderRect.bottom - borderRect.left;

		m_Window = CreateWindowExA(windowExStyle, "owl_window_class", m_Data.Title, windowStyle,
									windowX, windowY, windowWidth, windowHeight,
									nullptr, nullptr, m_Instance, nullptr);
		OWL_CORE_ASSERT(m_Window, GetLastError())
		SetWindowLongPtr(m_Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(m_Window, SW_SHOW);
		s_WindowCount++;
	}

	void WindowsWindow::Update()
	{
		OWL_PROFILE_FUNCTION();
		MSG message;
		while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
		{
			OWL_PROFILE_SCOPE("Process Message");
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}
	}

	LRESULT WindowsWindow::ProcessMessages(const uint32_t pMessage, WPARAM pWParam, LPARAM pLParam)
	{
		OWL_PROFILE_FUNCTION();
		
		switch (pMessage)
		{
		case WM_CLOSE:
			{
				WindowCloseEvent event;
				m_Data.EventCallback(event);
				
				return 1;
			}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SIZE:
			{
				RECT r;
				GetClientRect(m_Window, &r);
				m_Data.Width = r.right - r.left;
				m_Data.Height = r.bottom - r.top;

				if (!m_Data.EventCallback) break;
				WindowResizeEvent event(m_Data.Width, m_Data.Height);
				m_Data.EventCallback(event);
			}
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			{
				if ((pLParam & (1 << 30)) != 0)
					KeyPressedEvent event(static_cast<KeyCode>(pWParam), true);
				else if (pMessage == WM_KEYDOWN || pMessage == WM_SYSKEYDOWN)
				{
					KeyPressedEvent event(static_cast<KeyCode>(pWParam), false);
					Input::s_Keys[static_cast<KeyCode>(pWParam)] = true;
				}
				else
				{
					KeyReleasedEvent event(static_cast<KeyCode>(pWParam));
					Input::s_Keys[static_cast<KeyCode>(pWParam)] = false;
				}
			}
			break;
		case WM_CHAR:
			{
				KeyTypedEvent event(static_cast<char>(pWParam));
				m_Data.EventCallback(event);
			}
			break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			{
				MouseButtonPressedEvent event(static_cast<MouseCode>(pWParam));
				m_Data.EventCallback(event);
				Input::s_MouseButtons[static_cast<MouseCode>(pWParam)] = true;
			}
			break;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			{
				MouseButtonReleasedEvent event(static_cast<MouseCode>(pWParam));
				m_Data.EventCallback(event);
				Input::s_MouseButtons[static_cast<MouseCode>(pWParam)] = false;
			}
			break;
		case WM_MOUSEWHEEL:
			{
				MouseScrolledEvent event(static_cast<float>(GET_X_LPARAM(pLParam)), static_cast<float>(GET_X_LPARAM(pLParam)));
				m_Data.EventCallback(event);
			}
			break;
		case WM_MOUSEMOVE:
			{
				MouseMovedEvent event(static_cast<float>(GET_X_LPARAM(pLParam)), static_cast<float>(GET_X_LPARAM(pLParam)));
				m_Data.EventCallback(event);
				Input::s_MousePosition = glm::vec2(event.GetX(), event.GetY());
			}
			break;
		}

		return DefWindowProcA(m_Window, pMessage, pWParam, pLParam);
	}

	LRESULT WindowsWindow::WindowProc(const HWND pWindow, const uint32_t pMessage, WPARAM pWParam, LPARAM pLParam)
	{
		if (auto* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(pWindow, GWLP_USERDATA)))
			return window->ProcessMessages(pMessage, pWParam, pLParam);
		else 
			return DefWindowProc(pWindow, pMessage, pWParam, pLParam);
	}
}
