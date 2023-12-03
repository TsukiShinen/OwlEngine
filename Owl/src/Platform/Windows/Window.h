#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Owl
{
	class Window
	{
	public:
		Window(int pWidth, int pHeight, std::string pName);
		~Window();
		void CreateWindowSurface(VkInstance pInstance, VkSurfaceKHR* pSurface);

		[[nodiscard]] VkExtent2D GetExtent() const
		{
			return {static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height)};
		}

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		[[nodiscard]] bool ShouldClose() const { return glfwWindowShouldClose(m_Window); }
		[[nodiscard]] bool WasWindowResized() const { return m_IsFramebufferResized; }
		void ResetWindowResizeFlags() { m_IsFramebufferResized = false; }
		[[nodiscard]] GLFWwindow* GetWindow() const { return m_Window; }

	private:
		static void FramebufferResizeCallback(GLFWwindow* pWindow, int pWidth, int pHeight);
		void InitializeWindow();

		int m_Width;
		int m_Height;
		bool m_IsFramebufferResized;

		std::string m_Name;

		GLFWwindow* m_Window;
	};
}
