#include "VulkanRendererApi.h"

#include "VulkanDevice.h"
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
		m_Context->Surface = Application::Get().GetWindow()->CreateVulkanSurface(this);
		m_Context->Device = new VulkanDevice(m_Context);
		m_Context->Swapchain = new VulkanSwapchain(m_Context, m_Context->FramebufferWidth, m_Context->FramebufferHeight);

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
		delete m_Context->Swapchain;
		delete m_Context->Device;
		vkDestroySurfaceKHR(m_Context->Instance, m_Context->Surface, m_Context->Allocator);
		vkDestroyInstance(m_Context->Instance, m_Context->Allocator);
		delete m_Context;
	}

	void VulkanRendererApi::Resize(glm::vec2 pSize)
	{
		OWL_PROFILE_FUNCTION();
	}

	void VulkanRendererApi::BeginFrame()
	{
		OWL_PROFILE_FUNCTION();
	}

	void VulkanRendererApi::EndFrame()
	{
		OWL_PROFILE_FUNCTION();
	}

	void VulkanRendererApi::InitializeInstance(const std::string& pApplicationName)
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
		requiredExtensions.emplace_back(Application::Get().GetWindow()->GetVulkanRequiredExtension());
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

		const VkResult result = vkCreateInstance(&createInfo,  m_Context->Allocator, &m_Context->Instance);
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
		OWL_ASSERT(func(m_Context->Instance, &debugCreateInfo, m_Context->Allocator, &m_Context->DebugMessenger) == VK_SUCCESS,
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
