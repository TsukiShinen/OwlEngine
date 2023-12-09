#include "VulkanDevice.h"

#include "Owl/Debug/Assert.h"
#include "Owl/Debug/Instrumentor.h"

namespace Owl
{
	VulkanDevice::VulkanDevice(VulkanContext* pContext)
		: m_Context(pContext)
	{
		OWL_PROFILE_FUNCTION();
		SelectPhysicalDevice();
		CreateLogicalDevice();
		GetQueues();
		DetectDepthFormat();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(m_LogicalDevice, m_Context->Allocator);
	}

	void VulkanDevice::QuerySwapchainSupport(const VkPhysicalDevice pDevice, SwapchainInfo& pSwapchainInfo) const
	{
		OWL_PROFILE_FUNCTION();
		auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice, m_Context->Surface,
		                                                        &pSwapchainInfo.Capabilities);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanDevice] Couldn't Get surface capabilities!")

		uint32_t formatsCount;
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, m_Context->Surface, &formatsCount,
		                                              nullptr);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanDevice] Couldn't Get surface formats!")
		pSwapchainInfo.Formats.resize(formatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, m_Context->Surface, &formatsCount,
		                                              pSwapchainInfo.Formats.data());

		uint32_t modesCount;
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, m_Context->Surface,
		                                                   &modesCount, nullptr);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanDevice] Couldn't Get surface present modes!")
		pSwapchainInfo.PresentModes.resize(modesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, m_Context->Surface,
		                                                   &modesCount,
		                                                   pSwapchainInfo.PresentModes.data());
	}

	void VulkanDevice::DetectDepthFormat()
	{
		OWL_PROFILE_FUNCTION();
		constexpr uint64_t candidateCount = 3;
		constexpr VkFormat candidates[3] = {
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT};

		constexpr uint32_t flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
		for (uint64_t i = 0; i < candidateCount; ++i) {
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, candidates[i], &properties);

			if ((properties.linearTilingFeatures & flags) == flags)
				m_DepthFormat = candidates[i];
			else if ((properties.optimalTilingFeatures & flags) == flags)
				m_DepthFormat = candidates[i];
		}
		if (!m_DepthFormat)
			throw std::runtime_error("[VulkanDevice] Failed to find a supported format!");
	}

	void VulkanDevice::SelectPhysicalDevice()
	{
		OWL_PROFILE_FUNCTION();
		uint32_t physicalDeviceCount = 0;
		auto result = vkEnumeratePhysicalDevices(m_Context->Instance, &physicalDeviceCount, nullptr);
		OWL_CORE_ASSERT(physicalDeviceCount > 0, "[VulkanDevice] No devices which support Vulkan were found!")

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		result = vkEnumeratePhysicalDevices(m_Context->Instance, &physicalDeviceCount, physicalDevices.data());
		OWL_CORE_ASSERT(physicalDeviceCount > 0, "[VulkanDevice] Failed to get physical devices!")

		for (uint32_t i = 0; i < physicalDeviceCount; ++i)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);

			VkPhysicalDeviceMemoryProperties memory;
			vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &memory);

			QueueFamilyIndices queueFamilyIndices;
			SwapchainInfo swapchainInfo;
			if (!DoPhysicalDeviceMeetRequirements(physicalDevices[i], &features, queueFamilyIndices,
			                                      swapchainInfo))
				continue;

			m_PhysicalDevice = physicalDevices[i];

			m_Properties = properties;
			m_Features = features;
			m_Memory = memory;

			m_QueueFamilyIndices = queueFamilyIndices;
			m_SwapchainInfo = swapchainInfo;
			break;
		}

		if (!m_PhysicalDevice)
			throw std::runtime_error("No physical devices were found which meet the requirements.");

		OWL_CORE_INFO("=== Vulkan physical device found!");
	}

	bool VulkanDevice::DoPhysicalDeviceMeetRequirements(const VkPhysicalDevice pDevice,
	                                                    const VkPhysicalDeviceFeatures* pFeatures,
	                                                    QueueFamilyIndices& pQueueFamilyIndices,
	                                                    SwapchainInfo& pSwapchainInfo)
	{
		OWL_PROFILE_FUNCTION();

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, 0);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, queueFamilies.data());

		uint8_t minTransferScore = 255;
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			uint8_t currentTransferScore = 0;

			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				pQueueFamilyIndices.GraphicsFamily = i;
				++currentTransferScore;
			}

			if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				pQueueFamilyIndices.ComputeFamily = i;
				++currentTransferScore;
			}

			if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				if (currentTransferScore <= minTransferScore)
				{
					minTransferScore = currentTransferScore;
					pQueueFamilyIndices.TransferFamily = i;
				}
			}

			VkBool32 supportsPresent = VK_FALSE;
			if (vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, m_Context->Surface, &supportsPresent) == VK_SUCCESS)
			{
				pQueueFamilyIndices.PresentFamily = i;
			}
			else
			{
				OWL_CORE_ERROR("[VulkanDevice] Does not support present family!");
				return false;
			}
		}

		if (!pQueueFamilyIndices.IsValid(m_PhysicalDeviceRequirement))
		{
			OWL_CORE_ERROR("[VulkanDevice] Missing queue family!");
			return false;
		}

		QuerySwapchainSupport(pDevice, pSwapchainInfo);

		if (pSwapchainInfo.Formats.size() < 1 || pSwapchainInfo.PresentModes.size() < 1)
		{
			OWL_CORE_ERROR("[VulkanDevice] SwapchainInfo not valid!");
			return false;
		}

		if (!m_PhysicalDeviceRequirement.ExtensionNames.empty())
		{
			uint32_t availableExtensionCount = 0;
			std::vector<VkExtensionProperties> availableExtensions;
			auto result = vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &availableExtensionCount, nullptr);
			OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanDevice] Couldn't get Extension properties count!")
			if (availableExtensionCount != 0)
			{
				availableExtensions.resize(availableExtensionCount);
				result = vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &availableExtensionCount,
				                                              availableExtensions.data());
				OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanDevice] Couldn't get Extension properties!")

				const uint32_t requiredExtensionCount = m_PhysicalDeviceRequirement.ExtensionNames.size();
				for (uint32_t i = 0; i < requiredExtensionCount; ++i)
				{
					bool found = false;
					for (uint32_t j = 0; j < availableExtensionCount; ++j)
					{
						if (strcmp(m_PhysicalDeviceRequirement.ExtensionNames[i], availableExtensions[j].extensionName)
							!= 0)
						{
							found = true;
							break;
						}
					}

					if (!found)
					{
						OWL_CORE_ERROR("Required extension not found: {}!",
						               m_PhysicalDeviceRequirement.ExtensionNames[i]);
						return false;
					}
				}
			}
		}

		if (m_PhysicalDeviceRequirement.SamplerAnisotropy && !pFeatures->samplerAnisotropy)
		{
			OWL_CORE_ERROR("Device does not support samplerAnisotropy!");
			return FALSE;
		}

		return true;
	}

	void VulkanDevice::CreateLogicalDevice()
	{
		OWL_PROFILE_FUNCTION();
		const bool presentSharesGraphicsQueue = !m_QueueFamilyIndices.HaveSeparatePresentFamily();
		const bool transferSharesGraphicsQueue = !m_QueueFamilyIndices.HaveSeparateTransferFamily();
		uint32_t indexCount = 1;
		if (!presentSharesGraphicsQueue)
			indexCount++;
		if (!transferSharesGraphicsQueue)
			indexCount++;
		std::vector<uint32_t> indices(indexCount);
		uint8_t index = 0;
		indices[index++] = m_QueueFamilyIndices.GraphicsFamily;
		if (!presentSharesGraphicsQueue)
			indices[index++] = m_QueueFamilyIndices.PresentFamily;
		if (!transferSharesGraphicsQueue)
			indices[index] = m_QueueFamilyIndices.TransferFamily;

		float queuePriority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(indexCount);
		for (uint32_t i = 0; i < indexCount; ++i) {
			queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[i].queueFamilyIndex = indices[i];
			queueCreateInfos[i].queueCount = 1;
			queueCreateInfos[i].pQueuePriorities = &queuePriority;
			queueCreateInfos[i].flags = 0;
			queueCreateInfos[i].pNext = nullptr;
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = m_PhysicalDeviceRequirement.SamplerAnisotropy;

		VkDeviceCreateInfo deviceCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
		deviceCreateInfo.queueCreateInfoCount = indexCount;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		auto extensionNames = m_PhysicalDeviceRequirement.ExtensionNames;
		deviceCreateInfo.enabledExtensionCount = static_cast<glm::uint32_t>(extensionNames.size());
		deviceCreateInfo.ppEnabledExtensionNames = extensionNames.data();

		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = nullptr;

		if (auto result = vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, m_Context->Allocator, &m_LogicalDevice); result != VK_SUCCESS)
		{
			throw std::runtime_error("Couldn't create vulkan logical device!");
		}

		OWL_CORE_INFO("=== Vulkan Logical device created.");
	}

	void VulkanDevice::GetQueues()
	{
		vkGetDeviceQueue(
		m_LogicalDevice,
		m_QueueFamilyIndices.GraphicsFamily,
		0,
		&m_Graphics);

		vkGetDeviceQueue(
			m_LogicalDevice,
			m_QueueFamilyIndices.PresentFamily,
			0,
			&m_Present);

		vkGetDeviceQueue(
			m_LogicalDevice,
			m_QueueFamilyIndices.TransferFamily,
			0,
			&m_Transfer);
		OWL_CORE_INFO("=== Vulkan Queues obtained.");
	}
}
