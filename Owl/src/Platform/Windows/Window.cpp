#include "opch.h"
#include "Window.h"

#include <stdexcept>

namespace Owl
{
	Window::Window(const int pWidth, const int pHeight, std::string pName)
		: m_Width(pWidth), m_Height(pHeight), m_Name(std::move(pName))
	{
		InitializeWindow();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::CreateWindowSurface(VkInstance pInstance, VkSurfaceKHR* pSurface)
	{
		if (glfwCreateWindowSurface(pInstance, m_Window, nullptr, pSurface) != VK_SUCCESS)
			throw std::runtime_error("failed to create window surface");
	}

	void Window::FramebufferResizeCallback(GLFWwindow* pWindow, const int pWidth, const int pHeight)
	{
		const auto window = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));
		window->m_IsFramebufferResized = true;
		window->m_Width = pWidth;
		window->m_Height = pHeight;
	}

	void Window::InitializeWindow()
	{
		OWL_PROFILE_FUNCTION();
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);
	}
}
