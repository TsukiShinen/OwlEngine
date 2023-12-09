#include "opch.h"
#include "VulkanContext.h"

#include "VulkanCommandBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "Owl/Core/Application.h"

namespace Owl
{
	void VulkanContext::Initialize()
	{
		OWL_PROFILE_FUNCTION();
		Surface = Application::Get()->GetWindow()->CreateVulkanSurface(this);
		Device = new VulkanDevice(this);
		Swapchain = new VulkanSwapchain(this, FramebufferWidth, FramebufferHeight);
		MainRenderPass = new VulkanRenderPass(this, {0, 0, FramebufferWidth, FramebufferHeight}, 1, 0);
		CreateCommandBuffers();
	}

	VulkanContext::~VulkanContext()
	{
		OWL_PROFILE_FUNCTION();
		for (const auto& graphicsCommandBuffer : GraphicsCommandBuffers)
			delete graphicsCommandBuffer;
		delete MainRenderPass;
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

	void VulkanContext::CreateCommandBuffers()
	{
		OWL_PROFILE_FUNCTION();
		GraphicsCommandBuffers.resize(Swapchain->GetImageCount());

		for (uint32_t i = 0; i < Swapchain->GetImageCount(); ++i) {
			GraphicsCommandBuffers[i] = new VulkanCommandBuffer(this, Device->GetGraphicsCommandPool(), true);
		}
	}
}
