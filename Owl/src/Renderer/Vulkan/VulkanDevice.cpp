#include "VulkanDevice.h"

#include "Owl/Debug/Assert.h"
#include "Owl/Debug/Instrumentor.h"

namespace Owl
{
	VulkanDevice::VulkanDevice(const VkInstance pInstance, const VkSurfaceKHR pSurface)
	{
		OWL_PROFILE_FUNCTION();
		SelectPhysicalDevice(pInstance, pSurface);
	}

	void VulkanDevice::QuerySwapchainSupport(const VkPhysicalDevice pDevice, const VkSurfaceKHR pSurface,
	                                         SwapchainInfo& pSwapchainInfo)
	{
		OWL_PROFILE_FUNCTION();
		auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDevice, pSurface,
		                                                        &pSwapchainInfo.Capabilities);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanDevice] Couldn't Get surface capabilities!")

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, pSurface, &pSwapchainInfo.FormatCount,
		                                              nullptr);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanDevice] Couldn't Get surface format count!")
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(pDevice, pSurface, &pSwapchainInfo.FormatCount,
		                                              pSwapchainInfo.Formats.data());
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanDevice] Couldn't Get surface formats!")

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, pSurface,
		                                                   &pSwapchainInfo.PresentModeCount, nullptr);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanDevice] Couldn't Get surface present mode count!")
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(pDevice, pSurface,
		                                                   &pSwapchainInfo.PresentModeCount,
		                                                   pSwapchainInfo.PresentModes.data());
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanDevice] Couldn't Get surface present modes!")
	}

	void VulkanDevice::SelectPhysicalDevice(const VkInstance pInstance, const VkSurfaceKHR pSurface)
	{
		OWL_PROFILE_FUNCTION();
		uint32_t physicalDeviceCount = 0;
		auto result = vkEnumeratePhysicalDevices(pInstance, &physicalDeviceCount, nullptr);
		OWL_CORE_ASSERT(physicalDeviceCount > 0, "[VulkanDevice] No devices which support Vulkan were found!")

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		result = vkEnumeratePhysicalDevices(pInstance, &physicalDeviceCount, physicalDevices.data());
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
			if (!DoPhysicalDeviceMeetRequirements(physicalDevices[i], pSurface, &features, queueFamilyIndices,
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

		OWL_CORE_INFO("Vulkan physical device found!");
	}

	bool VulkanDevice::DoPhysicalDeviceMeetRequirements(const VkPhysicalDevice pDevice, const VkSurfaceKHR pSurface,
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
			if (vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, pSurface, &supportsPresent) == VK_SUCCESS)
			{
				pQueueFamilyIndices.PresentFamily = i;
			}
			else
			{
				OWL_CORE_ERROR("[VulkanDevice] Does not support present family!");
				return false;
			}
		}

		if (!pQueueFamilyIndices.IsValid())
		{
			OWL_CORE_ERROR("[VulkanDevice] Missing queue family!");
			return false;
		}

		QuerySwapchainSupport(pDevice, pSurface, pSwapchainInfo);

		if (pSwapchainInfo.FormatCount < 1 || pSwapchainInfo.PresentModeCount < 1)
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
}
