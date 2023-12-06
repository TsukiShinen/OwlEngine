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

		m_Instance = GetModuleHandle(nullptr);
		OWL_CORE_ASSERT(m_Instance, "[WindowsWindow] Failed to create window instance!")

		WNDCLASS windowClass = {};
		windowClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = m_Instance;
		windowClass.hIcon = LoadIcon(m_Instance, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = nullptr;
		windowClass.lpszClassName = L"owl_window_class";

		if (!RegisterClass(&windowClass))
		{
			throw std::runtime_error("[PlatformWindows] Failed to register window!");
		}

		uint32_t windowStyle = WS_OVERLAPPEDWINDOW;

		RECT clientRect = { 0, 0, m_Data.Width, m_Data.Height };
		AdjustWindowRectEx(&clientRect, windowStyle, FALSE, WS_EX_OVERLAPPEDWINDOW);

		const uint32_t windowX = 100;
		const uint32_t windowY = 100;

		m_Window = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,
			L"owl_window_class",
			std::wstring(m_Data.Title.begin(), m_Data.Title.end()).c_str(),
			windowStyle,
			windowX, windowY,
			clientRect.right - clientRect.left,
			clientRect.bottom - clientRect.top,
			nullptr, nullptr, m_Instance, nullptr);
		OWL_CORE_ASSERT(m_Window, GetLastError())
		SetWindowLongPtr(m_Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(m_Window, SW_SHOW);
		s_WindowCount++;

		OWL_CORE_INFO("Window initialized successfully : {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);
	}

	void WindowsWindow::Update()
	{
		OWL_PROFILE_FUNCTION();
		MSG message;
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			OWL_PROFILE_SCOPE("Process Message");
			TranslateMessage(&message);
			DispatchMessage(&message);
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
			{
				if ((pLParam & (1 << 30)) != 0)
					KeyPressedEvent event(static_cast<KeyCode>(pWParam), true);
				else
				{
					KeyPressedEvent event(static_cast<KeyCode>(pWParam), false);
					m_Data.EventCallback(event);
					Input::s_Keys[static_cast<KeyCode>(pWParam)] = true;
				}
		case WM_KEYUP:
		case WM_SYSKEYUP:
			{
				KeyReleasedEvent event(static_cast<KeyCode>(pWParam));
				m_Data.EventCallback(event);
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

		return DefWindowProc(m_Window, pMessage, pWParam, pLParam);
	}

	LRESULT WindowsWindow::WindowProc(const HWND pWindow, const uint32_t pMessage, WPARAM pWParam, LPARAM pLParam)
	{
		if (auto* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(pWindow, GWLP_USERDATA)))
			return window->ProcessMessages(pMessage, pWParam, pLParam);
		else 
			return DefWindowProc(pWindow, pMessage, pWParam, pLParam);
	}
}
