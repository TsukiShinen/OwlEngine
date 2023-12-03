#include "opch.h"
#include "Device.h"

#include <set>
#include <unordered_set>

namespace Owl::Renderer::Vulkan
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		const VkDebugUtilsMessageSeverityFlagBitsEXT pMessageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT pMessageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (pMessageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			OWL_CORE_ERROR(pCallbackData->pMessage);
		else if (pMessageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			OWL_CORE_WARN(pCallbackData->pMessage);
		else if (pMessageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			OWL_CORE_INFO(pCallbackData->pMessage);
		else if (pMessageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			OWL_CORE_TRACE(pCallbackData->pMessage);

		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerExt(
		const VkInstance pInstance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
			pInstance,
			"vkCreateDebugUtilsMessengerEXT"));
		if (func != nullptr)
			return func(pInstance, pCreateInfo, pAllocator, pDebugMessenger);
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void DestroyDebugUtilsMessengerExt(
		const VkInstance pInstance,
		const VkDebugUtilsMessengerEXT pDebugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
			pInstance,
			"vkDestroyDebugUtilsMessengerEXT"));
		if (func != nullptr)
			func(pInstance, pDebugMessenger, pAllocator);
	}

	Device::Device(Scope<Window>& pWindow)
		: m_Window(pWindow)
	{
		OWL_PROFILE_FUNCTION();
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateCommandPools();
	}

	Device::~Device()
	{
		OWL_PROFILE_FUNCTION();
		vkDestroyCommandPool(m_Device, m_GraphicsCommandPool, nullptr);
		vkDestroyCommandPool(m_Device, m_TransferCommandPool, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		if (m_EnableValidationLayers)
		{
			DestroyDebugUtilsMessengerExt(m_Instance, m_DebugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	uint32_t Device::FindMemoryType(const uint32_t pTypeFilter, const VkMemoryPropertyFlags pProperties) const
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((pTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & pProperties) == pProperties)
				return i;
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	VkFormat Device::FindSupportedFormat(const std::vector<VkFormat>& pCandidates, const VkImageTiling pTiling,
	                                     const VkFormatFeatureFlags pFeatures) const
	{
		for (const VkFormat format : pCandidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

			if (pTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & pFeatures) == pFeatures)
				return format;
			if (pTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & pFeatures) == pFeatures)
				return format;
		}
		throw std::runtime_error("failed to find supported format!");
	}

	void Device::CreateBuffer(const VkDeviceSize pSize, const VkBufferUsageFlags pUsage,
	                          const VkMemoryPropertyFlags pProperties,
	                          VkBuffer& pBuffer, VkDeviceMemory& pBufferMemory) const
	{
		const auto indices = FindQueueFamilies(m_PhysicalDevice);

		// Create Buffer
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = pSize;
		bufferInfo.usage = pUsage;
		bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;

		const uint32_t queueFamilyIndices[] = {indices.GraphicsFamily.value(), indices.TransferFamily.value()};
		bufferInfo.queueFamilyIndexCount = 2;
		bufferInfo.pQueueFamilyIndices = queueFamilyIndices;

		if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &pBuffer) != VK_SUCCESS)
			throw std::runtime_error("failed to create vertex buffer!");

		// Get and link memory
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_Device, pBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, pProperties);

		if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &pBufferMemory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate vertex buffer memory!");

		vkBindBufferMemory(m_Device, pBuffer, pBufferMemory, 0);
	}

	VkCommandBuffer Device::BeginSingleTimeGraphicsCommands() const
	{
		return BeginSingleTimeCommands(m_GraphicsCommandPool);
	}

	void Device::EndSingleTimeGraphicsCommands(const VkCommandBuffer pCommandBuffer) const
	{
		EndSingleTimeCommands(pCommandBuffer, m_GraphicsCommandPool, m_GraphicsQueue);
	}

	VkCommandBuffer Device::BeginSingleTimeTransferCommands() const
	{
		return BeginSingleTimeCommands(m_TransferCommandPool);
	}

	void Device::EndSingleTimeTransferCommands(const VkCommandBuffer pCommandBuffer) const
	{
		EndSingleTimeCommands(pCommandBuffer, m_TransferCommandPool, m_TransferQueue);
	}

	VkCommandBuffer Device::BeginSingleTimeCommands(const VkCommandPool& pCommandPool) const
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = pCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	void Device::EndSingleTimeCommands(const VkCommandBuffer pCommandBuffer, const VkCommandPool& pCommandPool,
	                                   const VkQueue& pQueue) const
	{
		vkEndCommandBuffer(pCommandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &pCommandBuffer;

		vkQueueSubmit(pQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(pQueue);

		vkFreeCommandBuffers(m_Device, pCommandPool, 1, &pCommandBuffer);
	}

	void Device::CopyBuffer(const VkBuffer pSrcBuffer, const VkBuffer pDstBuffer, const VkDeviceSize pSize) const
	{
		const VkCommandBuffer commandBuffer = BeginSingleTimeTransferCommands();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = pSize;
		vkCmdCopyBuffer(commandBuffer, pSrcBuffer, pDstBuffer, 1, &copyRegion);

		EndSingleTimeTransferCommands(commandBuffer);
	}

	void Device::CopyBufferToImage(const VkBuffer pBuffer, const VkImage pImage, const uint32_t pWidth,
	                               const uint32_t pHeight) const
	{
		const VkCommandBuffer commandBuffer = BeginSingleTimeTransferCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = {0, 0, 0};
		region.imageExtent = {
			pWidth,
			pHeight,
			1
		};

		vkCmdCopyBufferToImage(
			commandBuffer,
			pBuffer,
			pImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		EndSingleTimeTransferCommands(commandBuffer);
	}

	void Device::CreateImageWithInfo(const VkImageCreateInfo& pImageInfo, VkMemoryPropertyFlags pProperties,
	                                 VkImage& pImage, VkDeviceMemory& pImageMemory) const
	{
		if (vkCreateImage(m_Device, &pImageInfo, nullptr, &pImage) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_Device, pImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, pProperties);

		if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &pImageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}

		if (vkBindImageMemory(m_Device, pImage, pImageMemory, 0) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to bind image memory!");
		}
	}

	VkImageView Device::CreateImageView(const VkImage pImage, const VkFormat pFormat) const
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = pImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = pFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			throw std::runtime_error("failed to create texture image view!");

		return imageView;
	}

	void Device::CreateInstance()
	{
		OWL_PROFILE_FUNCTION();
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "LittleVulkanEngine App";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		const auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (m_EnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = &debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}

		HasGflwRequiredInstanceExtensions();
	}

	void Device::SetupDebugMessenger()
	{
		OWL_PROFILE_FUNCTION();
		if (!m_EnableValidationLayers) return;
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);
		if (CreateDebugUtilsMessengerExt(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void Device::CreateSurface()
	{
		OWL_PROFILE_FUNCTION();
		m_Window->CreateWindowSurface(m_Instance, &m_Surface);
	}

	void Device::PickPhysicalDevice()
	{
		OWL_PROFILE_FUNCTION();
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
		if (deviceCount == 0)
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		OWL_CORE_INFO("===== Device count: {}", deviceCount);

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				m_PhysicalDevice = device;
				break;
			}
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("failed to find a suitable GPU!");

		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &Properties);
		OWL_CORE_INFO("Physical device: {}", Properties.deviceName);
	}

	void Device::CreateLogicalDevice()
	{
		OWL_PROFILE_FUNCTION();
		QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set uniqueQueueFamilies = {
			indices.GraphicsFamily.value(), indices.PresentFamily.value(), indices.TransferFamily.value()
		};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

		if (m_EnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
		}
		else
			createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
			throw std::runtime_error("failed to create logical device!");

		vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, indices.PresentFamily.value(), 0, &m_PresentQueue);
		vkGetDeviceQueue(m_Device, indices.TransferFamily.value(), 0, &m_TransferQueue);
	}

	void Device::CreateCommandPools()
	{
		OWL_PROFILE_FUNCTION();
		const QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_PhysicalDevice);

		VkCommandPoolCreateInfo graphicsPoolInfo{};
		graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		graphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		graphicsPoolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

		if (vkCreateCommandPool(m_Device, &graphicsPoolInfo, nullptr, &m_GraphicsCommandPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create command pool!");

		VkCommandPoolCreateInfo transferPoolInfo{};
		transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		transferPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		transferPoolInfo.queueFamilyIndex = queueFamilyIndices.TransferFamily.value();

		if (vkCreateCommandPool(m_Device, &transferPoolInfo, nullptr, &m_TransferCommandPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create command pool!");
	}

	bool Device::IsDeviceSuitable(VkPhysicalDevice pDevice) const
	{
		QueueFamilyIndices indices = FindQueueFamilies(pDevice);

		bool extensionsSupported = CheckDeviceExtensionSupport(pDevice);

		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(pDevice);
			swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(pDevice, &supportedFeatures);

		return indices.IsComplete() && extensionsSupported && swapChainAdequate &&
			supportedFeatures.samplerAnisotropy;
	}

	std::vector<const char*> Device::GetRequiredExtensions() const
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_EnableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	bool Device::CheckValidationLayerSupport() const
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : m_ValidationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	QueueFamilyIndices Device::FindQueueFamilies(const VkPhysicalDevice pDevice) const
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.GraphicsFamily = i;
			else if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
				indices.TransferFamily = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, m_Surface, &presentSupport);

			if (presentSupport)
				indices.PresentFamily = i;

			if (indices.IsComplete())
				break;

			i++;
		}

		return indices;
	}

	void Device::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& pCreateInfo)
	{
		pCreateInfo = {};
		pCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		pCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		pCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		pCreateInfo.pfnUserCallback = DebugCallback;
		pCreateInfo.pUserData = nullptr; // Optional
	}

	void Device::HasGflwRequiredInstanceExtensions() const
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::unordered_set<std::string> available;
		for (const auto& extension : extensions)
		{
			available.insert(extension.extensionName);
		}

		const auto requiredExtensions = GetRequiredExtensions();
		for (const auto& required : requiredExtensions)
		{
			if (!available.contains(required))
			{
				throw std::runtime_error("Missing required glfw extension");
			}
		}
	}

	bool Device::CheckDeviceExtensionSupport(const VkPhysicalDevice pDevice) const
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(
			pDevice,
			nullptr,
			&extensionCount,
			availableExtensions.data());

		std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapChainSupportDetails Device::QuerySwapChainSupport(const VkPhysicalDevice pDevice) const
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice, m_Surface, &details.Capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, m_Surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, m_Surface, &formatCount, details.Formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, m_Surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, m_Surface, &presentModeCount,
			                                          details.PresentModes.data());
		}

		return details;
	}
}
