#include "opch.h"
#include "VulkanContext.h"

#include "VulkanCommandBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"
#include "VulkanFence.h"
#include "VulkanSwapchain.h"
#include "Owl/Core/Application.h"

namespace Owl
{

	void VulkanContext::Initialize()
	{
		OWL_PROFILE_FUNCTION();

		auto sizeFrameBuffer = Application::Get()->GetFrameBufferSize();
		FramebufferWidth = sizeFrameBuffer.x;
		FramebufferHeight = sizeFrameBuffer.y;
		
		Surface = Application::Get()->GetWindow()->CreateVulkanSurface(this);
		Device = new VulkanDevice(this);
		Swapchain = new VulkanSwapchain(this, FramebufferWidth, FramebufferHeight);
		MainRenderPass = new VulkanRenderPass(this, {0, 0, FramebufferWidth, FramebufferHeight}, 1, 0);
		RegenerateFrameBuffers();
		CreateCommandBuffers();
		CreateSemaphoresAndFences();
	}

	VulkanContext::~VulkanContext()
	{
		OWL_PROFILE_FUNCTION();
		vkDeviceWaitIdle(Device->GetLogicalDevice());
		
		for (const auto& semaphore : ImageAvailableSemaphore)
			vkDestroySemaphore(Device->GetLogicalDevice(), semaphore, Allocator);
		for (const auto& semaphore : QueueCompleteSemaphore)
			vkDestroySemaphore(Device->GetLogicalDevice(), semaphore, Allocator);
		for (const auto& fence : InFlightFences)
			delete fence;
		for (const auto& graphicsCommandBuffer : GraphicsCommandBuffers)
			delete graphicsCommandBuffer;
		for (const auto& framebuffer : Swapchain->m_FrameBuffers)
			delete framebuffer;
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

	void VulkanContext::RegenerateFrameBuffers()
	{
		Swapchain->m_FrameBuffers.resize(Swapchain->GetImageCount());
		for (uint32_t i = 0; i < Swapchain->GetImageCount(); ++i)
		{
			// TODO: make this dynamic based on the currently configured attachments
			std::vector attachments = {
				Swapchain->m_Views[i],
				Swapchain->m_DepthAttachment->m_View
			};

			Swapchain->m_FrameBuffers[i] = new VulkanFrameBuffer(this, MainRenderPass, FramebufferWidth, FramebufferHeight, attachments);
		}
	}

	void VulkanContext::CreateCommandBuffers()
	{
		OWL_PROFILE_FUNCTION();
		GraphicsCommandBuffers.resize(Swapchain->GetImageCount());

		for (uint32_t i = 0; i < Swapchain->GetImageCount(); ++i) {
			GraphicsCommandBuffers[i] = new VulkanCommandBuffer(this, Device->GetGraphicsCommandPool(), true);
		}
	}
	
	void VulkanContext::CreateSemaphoresAndFences()
	{
		ImageAvailableSemaphore.resize(Swapchain->GetMaxFrameInFlight());
		QueueCompleteSemaphore.resize(Swapchain->GetMaxFrameInFlight());
		InFlightFences.resize(Swapchain->GetMaxFrameInFlight());

		for (uint8_t i = 0; i < Swapchain->GetMaxFrameInFlight(); ++i)
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
			vkCreateSemaphore(Device->GetLogicalDevice(), &semaphoreCreateInfo, Allocator, &ImageAvailableSemaphore[i]);
			vkCreateSemaphore(Device->GetLogicalDevice(), &semaphoreCreateInfo, Allocator, &QueueCompleteSemaphore[i]);

			InFlightFences[i] = new VulkanFence(this, true);
		}
	}
}
