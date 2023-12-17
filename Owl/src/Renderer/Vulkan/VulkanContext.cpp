#include "opch.h"
#include "VulkanContext.h"

#include "VulkanCommandBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"
#include "VulkanFence.h"
#include "VulkanSwapchain.h"
#include "Owl/Core/Application.h"
#include "Shaders/VulkanSpriteShader.h"

namespace Owl
{
	void VulkanContext::Initialize()
	{
		OWL_PROFILE_FUNCTION();

		const auto sizeFrameBuffer = Application::Get()->GetFrameBufferSize();
		FramebufferWidth = sizeFrameBuffer.x;
		FramebufferHeight = sizeFrameBuffer.y;

		Surface = Application::Get()->GetWindow()->CreateVulkanSurface(this);
		Device = new VulkanDevice(this);
		Swapchain = new VulkanSwapchain(this, FramebufferWidth, FramebufferHeight);
		MainRenderPass = new VulkanRenderPass(this, {0, 0, static_cast<float>(FramebufferWidth), static_cast<float>(FramebufferHeight)});
		RegenerateFrameBuffers();
		CreateCommandBuffers();
		CreateSemaphoresAndFences();
		ImageIndex = 0;

		// Create BuiltinShaders
		SpriteShader = new VulkanSpriteShader(this);
	}

	VulkanContext::~VulkanContext()
	{
		OWL_PROFILE_FUNCTION();
		vkDeviceWaitIdle(Device->GetLogicalDevice());

		delete SpriteShader;
		
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

	bool VulkanContext::RecreateSwapChain()
	{
		if (IsRecreatingSwapchain)
			return false;

		if (FramebufferWidth == 0 || FramebufferHeight == 0)
		{
			Application::Get()->SetMinimized(true);
			return false;
		}
		Application::Get()->SetMinimized(false);

		IsRecreatingSwapchain = true;

		vkDeviceWaitIdle(Device->GetLogicalDevice());

		for (uint32_t i = 0; i < Swapchain->GetImageCount(); ++i)
		{
			ImagesInFlight[i] = nullptr;
		}

		Device->QuerySwapchainSupport(Device->GetPhysicalDevice(), Device->GetSwapchainInfo());
		Device->DetectDepthFormat();

		Swapchain->ReCreate(FramebufferWidth, FramebufferHeight);

		MainRenderPass->SetWidth(FramebufferWidth);
		MainRenderPass->SetHeight(FramebufferHeight);

		for (uint32_t i = 0; i < Swapchain->GetImageCount(); ++i)
		{
			delete GraphicsCommandBuffers[i];
			delete Swapchain->GetFrameBufferAt(i);
		}

		RegenerateFrameBuffers();
		CreateCommandBuffers();

		IsRecreatingSwapchain = false;
		NeedNewSwapchain = false;

		return true;
	}

	int VulkanContext::FindMemoryIndex(const uint32_t pTypeFilter, const VkFlags pPropertyFlags) const
	{
		OWL_PROFILE_FUNCTION();
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(Device->GetPhysicalDevice(), &memoryProperties);

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
		{
			// Check each memory type to see if its bit is set to 1.
			if (pTypeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & pPropertyFlags) ==
				pPropertyFlags)
			{
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

			Swapchain->m_FrameBuffers[i] = new VulkanFrameBuffer(this, MainRenderPass, FramebufferWidth,
			                                                     FramebufferHeight, attachments);
		}
	}

	void VulkanContext::CreateCommandBuffers()
	{
		OWL_PROFILE_FUNCTION();
		GraphicsCommandBuffers.resize(Swapchain->GetImageCount());

		for (uint32_t i = 0; i < Swapchain->GetImageCount(); ++i)
		{
			GraphicsCommandBuffers[i] = new VulkanCommandBuffer(this, Device->GetGraphicsCommandPool(), true);
		}
	}

	void VulkanContext::CreateSemaphoresAndFences()
	{
		ImageAvailableSemaphore.resize(Swapchain->GetMaxFrameInFlight());
		QueueCompleteSemaphore.resize(Swapchain->GetMaxFrameInFlight());
		InFlightFences.resize(Swapchain->GetMaxFrameInFlight());
		ImagesInFlight.resize(Swapchain->GetImageCount());

		for (uint8_t i = 0; i < Swapchain->GetMaxFrameInFlight(); ++i)
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
			vkCreateSemaphore(Device->GetLogicalDevice(), &semaphoreCreateInfo, Allocator, &ImageAvailableSemaphore[i]);
			vkCreateSemaphore(Device->GetLogicalDevice(), &semaphoreCreateInfo, Allocator, &QueueCompleteSemaphore[i]);

			InFlightFences[i] = new VulkanFence(this, true);
		}
	}
}
