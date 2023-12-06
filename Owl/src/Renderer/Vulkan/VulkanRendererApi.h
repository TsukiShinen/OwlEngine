﻿#pragma once
#include <string>

#include "Owl/Renderer/RendererApi.h"

#include <vulkan/vulkan.h>

#include "VulkanContext.h"
#include "Owl/Core/Base.h"

namespace Owl
{
	class VulkanRendererApi : public RendererApi
	{
	public:
		VulkanRendererApi(const std::string& pApplicationName);
		~VulkanRendererApi() override;

		void Resize(glm::vec2 pSize) override;
		void BeginFrame() override;
		void EndFrame() override;

	private:
		void InitializeInstance(const std::string& pApplicationName);
#ifdef OWL_DEBUG
		void InitializeDebugMessage();
#endif

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT pMessageSeverity,
		                                                    VkDebugUtilsMessageTypeFlagsEXT pMessageTypes,
		                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		                                                    void* pUserData);

		VulkanContext* m_Context;

		friend class WindowsWindow;
	};
}
