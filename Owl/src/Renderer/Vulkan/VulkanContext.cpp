#include "opch.h"
#include "VulkanContext.h"

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "Owl/Core/Application.h"

namespace Owl
{
	void VulkanContext::Initialize()
	{
		Surface = Application::Get()->GetWindow()->CreateVulkanSurface(this);
		Device = new VulkanDevice(this);
		Swapchain = new VulkanSwapchain(this, FramebufferWidth, FramebufferHeight);
	}

	VulkanContext::~VulkanContext()
	{
		delete Swapchain;
		delete Device;
		vkDestroySurfaceKHR(Instance, Surface, Allocator);
		vkDestroyInstance(Instance, Allocator);
	}

	int VulkanContext::FindMemoryIndex(const uint32_t pTypeFilter, const VkFlags pPropertyFlags) const
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
