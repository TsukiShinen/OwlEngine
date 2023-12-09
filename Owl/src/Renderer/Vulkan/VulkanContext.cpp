#include "opch.h"
#include "VulkanContext.h"

#include "VulkanDevice.h"

namespace Owl
{
	int VulkanContext::FindMemoryIndex(const uint32_t pTypeFilter, const VkFlags pPropertyFlags)
	{
		OWL_PROFILE_FUNCTION();
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(Device->GetPhysicalDevice(), &memoryProperties);

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
			// Check each memory type to see if its bit is set to 1.
			if (pTypeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & pPropertyFlags) == pPropertyFlags) {
				return i;
			}
		}

		OWL_CORE_WARN("[VulkanContext] Unable to find suitable memory type!");
		return -1;
	}
}
