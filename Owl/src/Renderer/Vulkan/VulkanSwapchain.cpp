#include "VulkanSwapchain.h"

#include "VulkanDevice.h"
#include "Owl/Debug/Assert.h"
#include "Owl/Debug/Instrumentor.h"
#include "Owl/Debug/Log.h"

namespace Owl
{
	VulkanSwapchain::VulkanSwapchain(VulkanContext* pContext, const uint32_t pWidth, const uint32_t pHeight)
		: m_Context(pContext)
	{
		Create(pWidth, pHeight);

		OWL_CORE_INFO("=== Vulkan Swapchain created successfully.");
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		Destroy();
	}

	void VulkanSwapchain::ReCreate(const uint32_t pWidth, const uint32_t pHeight)
	{
		OWL_PROFILE_FUNCTION();
		Destroy();
		Create(pWidth, pHeight);
	}

	bool VulkanSwapchain::AcquireNextImage(const uint64_t pTimeoutNanoSecond, const VkSemaphore pImageSemaphore,
	                                       const VkFence pFence, uint32_t& pImageIndex)
	{
		OWL_PROFILE_FUNCTION();

		if (const VkResult result = vkAcquireNextImageKHR(
			m_Context->Device->GetLogicalDevice(),
			m_Handle,
			pTimeoutNanoSecond,
			pImageSemaphore,
			pFence,
			&pImageIndex); result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			ReCreate(m_Context->FramebufferWidth, m_Context->FramebufferHeight);
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			OWL_CORE_CRITICAL("[VulkanSwapchain] Failed to acquire swapchain image!");
			return false;
		}

		return true;
	}

	void VulkanSwapchain::Present(const VkSemaphore pRenderCompleted, const uint32_t pImageIndex)
	{
		OWL_PROFILE_FUNCTION();
		VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &pRenderCompleted;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Handle;
		presentInfo.pImageIndices = &pImageIndex;
		presentInfo.pResults = nullptr;

		VkResult result = vkQueuePresentKHR(m_Context->Device->GetPresentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			ReCreate(m_Context->FramebufferWidth, m_Context->FramebufferHeight);
		}
		else if (result != VK_SUCCESS)
		{
			OWL_CORE_CRITICAL("[VulkanSwapchain] Failed to present swap chain image!");
		}

		m_Context->CurrentFrame = (m_Context->CurrentFrame + 1) % m_MaxFrameInFlight;
	}

	void VulkanSwapchain::Create(const uint32_t pWidth, const uint32_t pHeight)
	{
		OWL_PROFILE_FUNCTION();
		VkExtent2D swapchainExtent{pWidth, pHeight};

		// Format
		m_ImageFormat = m_Context->Device->GetSwapchainInfo().Formats[0];
		for (const auto format : m_Context->Device->GetSwapchainInfo().Formats)
		{
			// Preferred formats
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
				format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				m_ImageFormat = format;
				break;
			}
		}

		// Mode
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto mode : m_Context->Device->GetSwapchainInfo().PresentModes)
		{
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				presentMode = mode;
				break;
			}
		}

		m_Context->Device->QuerySwapchainSupport(m_Context->Device->GetPhysicalDevice(),
		                                         m_Context->Device->GetSwapchainInfo());

		// Extent
		if (m_Context->Device->GetSwapchainInfo().Capabilities.currentExtent.width != UINT32_MAX)
		{
			swapchainExtent = m_Context->Device->GetSwapchainInfo().Capabilities.currentExtent;
		}

		// Clamp to the value allowed by the GPU.
		const VkExtent2D min = m_Context->Device->GetSwapchainInfo().Capabilities.minImageExtent;
		const VkExtent2D max = m_Context->Device->GetSwapchainInfo().Capabilities.maxImageExtent;
		swapchainExtent.width = Math::Clamp(swapchainExtent.width, min.width, max.width);
		swapchainExtent.height = Math::Clamp(swapchainExtent.height, min.height, max.height);

		uint32_t imageCount = m_Context->Device->GetSwapchainInfo().Capabilities.minImageCount + 1;
		if (m_Context->Device->GetSwapchainInfo().Capabilities.maxImageCount > 0 && imageCount > m_Context->Device->
			GetSwapchainInfo().Capabilities.maxImageCount)
		{
			imageCount = m_Context->Device->GetSwapchainInfo().Capabilities.maxImageCount;
		}
		m_MaxFrameInFlight = imageCount - 1;

		// Swapchain create info
		VkSwapchainCreateInfoKHR swapchainCreateInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
		swapchainCreateInfo.surface = m_Context->Surface;
		swapchainCreateInfo.minImageCount = imageCount;
		swapchainCreateInfo.imageFormat = m_ImageFormat.format;
		swapchainCreateInfo.imageColorSpace = m_ImageFormat.colorSpace;
		swapchainCreateInfo.imageExtent = swapchainExtent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// Setup the queue family indices
		if (m_Context->Device->GetQueueFamilyIndices().HaveSeparatePresentFamily())
		{
			const uint32_t queueFamilyIndices[] = {
				static_cast<uint32_t>(m_Context->Device->GetQueueFamilyIndices().GraphicsFamily),
				static_cast<uint32_t>(m_Context->Device->GetQueueFamilyIndices().PresentFamily)
			};
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		}

		swapchainCreateInfo.preTransform = m_Context->Device->GetSwapchainInfo().Capabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = nullptr;

		auto result = vkCreateSwapchainKHR(m_Context->Device->GetLogicalDevice(), &swapchainCreateInfo,
		                                   m_Context->Allocator, &m_Handle);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanSwapchain] Failed to create swapchain!")

		m_Context->CurrentFrame = 0;
		m_ImageCount = 0;
		result = vkGetSwapchainImagesKHR(m_Context->Device->GetLogicalDevice(), m_Handle, &m_ImageCount, nullptr);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanSwapchain] Failed to get image count!")
		if (!m_Images)
		{
			m_Images = static_cast<VkImage*>(OWL_ALLOCATE(sizeof(VkImage) * m_ImageCount, MemoryTagRenderer));
		}
		if (!m_Views)
		{
			m_Views = static_cast<VkImageView*>(OWL_ALLOCATE(sizeof(VkImageView) * m_ImageCount, MemoryTagRenderer));
		}
		result = vkGetSwapchainImagesKHR(m_Context->Device->GetLogicalDevice(), m_Handle, &m_ImageCount, m_Images);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanSwapchain] Failed to create images!")

		for (uint32_t i = 0; i < m_ImageCount; ++i)
		{
			VkImageViewCreateInfo viewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
			viewInfo.image = m_Images[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = m_ImageFormat.format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			result = vkCreateImageView(m_Context->Device->GetLogicalDevice(), &viewInfo, m_Context->Allocator,
			                           &m_Views[i]);
			OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanSwapchain] Failed to create views!")
		}

		// Create depth image and its view.
		m_DepthAttachment = new VulkanImage(m_Context, swapchainExtent.width, swapchainExtent.height,
		                                    m_Context->Device->GetDepthFormat(),
		                                    VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		                                    true, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void VulkanSwapchain::Destroy() const
	{
		OWL_PROFILE_FUNCTION();
		vkDeviceWaitIdle(m_Context->Device->GetLogicalDevice());
		delete m_DepthAttachment;

		for (uint32_t i = 0; i < m_ImageCount; ++i)
		{
			vkDestroyImageView(m_Context->Device->GetLogicalDevice(), m_Views[i], m_Context->Allocator);
		}
		OWL_FREE(m_Images, sizeof(VkImage) * m_ImageCount, MemoryTagRenderer);
		OWL_FREE(m_Views, sizeof(VkImageView) * m_ImageCount, MemoryTagRenderer);

		vkDestroySwapchainKHR(m_Context->Device->GetLogicalDevice(), m_Handle, m_Context->Allocator);
	}
}
