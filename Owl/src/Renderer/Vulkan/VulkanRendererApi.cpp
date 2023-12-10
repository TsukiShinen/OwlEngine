#include "VulkanRendererApi.h"

#include "VulkanDevice.h"
#include "VulkanFence.h"
#include "VulkanSwapchain.h"
#include "Owl/Core/Application.h"
#include "Owl/Debug/Assert.h"

namespace Owl
{
	VulkanRendererApi::VulkanRendererApi(const std::string& pApplicationName)
	{
		OWL_PROFILE_FUNCTION();
		OWL_CORE_INFO("========== Vulkan Renderer ==========");
		m_Context = new VulkanContext();
		// TODO: Custom allocator
		m_Context->Allocator = nullptr;

		InitializeInstance(pApplicationName);
		InitializeDebugMessage();
		m_Context->Initialize();

		OWL_CORE_INFO("=====================================");
	}

	VulkanRendererApi::~VulkanRendererApi()
	{
		OWL_PROFILE_FUNCTION();
#ifdef OWL_DEBUG
		if (m_Context->DebugMessenger)
		{
			const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
				m_Context->Instance, "vkDestroyDebugUtilsMessengerEXT"));
			func(m_Context->Instance, m_Context->DebugMessenger, m_Context->Allocator);
		}
#endif
		delete m_Context;
	}

	void VulkanRendererApi::Resize(const glm::vec2 pSize)
	{
		OWL_PROFILE_FUNCTION();
		m_Context->FramebufferWidth = static_cast<float>(pSize.x);
		m_Context->FramebufferHeight = static_cast<float>(pSize.y);
		m_Context->NeedNewSwapchain = true;
	}

	bool VulkanRendererApi::BeginFrame()
	{
		OWL_PROFILE_FUNCTION();
		if (m_Context->IsRecreatingSwapchain)
		{
			if (auto result = vkDeviceWaitIdle(m_Context->Device->GetLogicalDevice()); result != VK_SUCCESS)
				OWL_CORE_ERROR("[VulkanRendererApi] Failed to vkDeviceWaitIdle in BeginFrame : {0}", result);
			return false;
		}

		if (m_Context->NeedNewSwapchain)
		{
			if (auto result = vkDeviceWaitIdle(m_Context->Device->GetLogicalDevice()); result != VK_SUCCESS)
			{
				OWL_CORE_ERROR("[VulkanRendererApi] Failed to vkDeviceWaitIdle in BeginFrame : {0}", result);
				return false;
			}

			if (m_Context->RecreateSwapChain())
				OWL_CORE_INFO("=== Vulkan Swapchain resized succesfully!");

			return false;
		}

		m_Context->InFlightFences[m_Context->CurrentFrame]->Wait(UINT64_MAX);

		if (!m_Context->Swapchain->AcquireNextImage(
			UINT64_MAX, m_Context->ImageAvailableSemaphore[m_Context->CurrentFrame], nullptr, m_Context->ImageIndex))
			return false;

		VulkanCommandBuffer* commandBuffer = m_Context->GraphicsCommandBuffers[m_Context->ImageIndex];
		commandBuffer->Reset();
		commandBuffer->Begin(false, false);

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(m_Context->FramebufferHeight);
		viewport.width = static_cast<float>(m_Context->FramebufferWidth);
		viewport.height = -static_cast<float>(m_Context->FramebufferHeight);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// Scissor
		VkRect2D scissor;
		scissor.offset.x = scissor.offset.y = 0;
		scissor.extent.width = m_Context->FramebufferWidth;
		scissor.extent.height = m_Context->FramebufferHeight;

		vkCmdSetViewport(commandBuffer->GetHandle(), 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer->GetHandle(), 0, 1, &scissor);

		m_Context->MainRenderPass->Begin(commandBuffer,
		                                 m_Context->Swapchain->GetFrameBufferAt(m_Context->ImageIndex)->GetHandle());

		return true;
	}

	void VulkanRendererApi::EndFrame()
	{
		OWL_PROFILE_FUNCTION();

		VulkanCommandBuffer* commandBuffer = m_Context->GraphicsCommandBuffers[m_Context->ImageIndex];

		m_Context->MainRenderPass->End(commandBuffer);

		commandBuffer->End();

		if (m_Context->ImagesInFlight[m_Context->ImageIndex] != nullptr)
			m_Context->ImagesInFlight[m_Context->ImageIndex]->Wait(UINT64_MAX);

		m_Context->ImagesInFlight[m_Context->ImageIndex] = m_Context->InFlightFences[m_Context->CurrentFrame];

		m_Context->InFlightFences[m_Context->CurrentFrame]->Reset();

		VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer->GetHandle();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_Context->QueueCompleteSemaphore[m_Context->CurrentFrame];
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_Context->ImageAvailableSemaphore[m_Context->CurrentFrame];

		constexpr VkPipelineStageFlags flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.pWaitDstStageMask = flags;

		if (VkResult result = vkQueueSubmit(
			m_Context->Device->GetGraphicsQueue(),
			1,
			&submitInfo,
			m_Context->InFlightFences[m_Context->CurrentFrame]->GetHandle()); result != VK_SUCCESS)
		{
			OWL_CORE_ERROR("vkQueueSubmit failed with result: {0}", result);
			return;
		}

		commandBuffer->Submitted();

		m_Context->Swapchain->Present(m_Context->QueueCompleteSemaphore[m_Context->CurrentFrame],
		                              m_Context->ImageIndex);
	}

	void VulkanRendererApi::InitializeInstance(const std::string& pApplicationName) const
	{
		OWL_PROFILE_FUNCTION();
		VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
		appInfo.apiVersion = VK_API_VERSION_1_2;
		appInfo.pApplicationName = pApplicationName.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Owl Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

		VkInstanceCreateInfo createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
		createInfo.pApplicationInfo = &appInfo;

		// ===== Required Extensions =====
		std::vector<const char*> requiredExtensions{};
		requiredExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
		requiredExtensions.emplace_back(Application::Get()->GetWindow()->GetVulkanRequiredExtension());
#ifdef OWL_DEBUG
		requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		// ===== Validation Layer =====
		std::vector<const char*> requiredValidationLayer{};
#ifdef OWL_DEBUG
		requiredValidationLayer.emplace_back("VK_LAYER_KHRONOS_validation");
#endif
		createInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayer.size());
		createInfo.ppEnabledLayerNames = requiredValidationLayer.data();

		const VkResult result = vkCreateInstance(&createInfo, m_Context->Allocator, &m_Context->Instance);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanRendererApi] Failed to create vkInstance!")

		OWL_CORE_INFO("=== Vulkan Instance created successfully.");
	}

#ifdef OWL_DEBUG
	void VulkanRendererApi::InitializeDebugMessage()
	{
		OWL_PROFILE_FUNCTION();

		constexpr uint32_t logSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
		debugCreateInfo.messageSeverity = logSeverity;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugCallback;

		const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(m_Context->Instance, "vkCreateDebugUtilsMessengerEXT"));
		OWL_ASSERT(func, "[VulkanRendererApi] Failed to create debug messenger")
		OWL_ASSERT(
			func(m_Context->Instance, &debugCreateInfo, m_Context->Allocator, &m_Context->DebugMessenger) == VK_SUCCESS,
			"[VulkanRendererApi] Failed to create debug messenger")

		OWL_CORE_INFO("=== Vulkan debug message initialized successfully.");
	}
#endif

	VkBool32 VulkanRendererApi::DebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT pMessageSeverity,
	                                          VkDebugUtilsMessageTypeFlagsEXT pMessageTypes,
	                                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	                                          void* pUserData)
	{
		std::string error = "[Vulkan Debug] ";
		error += pCallbackData->pMessage;
		switch (pMessageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			OWL_CORE_ERROR(error);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			OWL_CORE_WARN(error);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			OWL_CORE_INFO(error);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			OWL_CORE_TRACE(error);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			break;
		}
		return VK_FALSE;
	}
}
