#pragma once

#include "VulkanContext.h"
#include "VulkanFrameBuffer.h"
#include "VulkanImage.h"

namespace Owl
{
	class VulkanSwapchain
	{
	public:
		
		VulkanSwapchain(VulkanContext* pContext, uint32_t pWidth, uint32_t pHeight);
		~VulkanSwapchain();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagRenderer); }

		void ReCreate(uint32_t pWidth, uint32_t pHeight);
		bool AcquireNextImage(uint64_t pTimeoutNanoSecond, VkSemaphore pImageSemaphore, VkFence pFence, uint32_t& pImageIndex);
		void Present(VkSemaphore pRenderCompleted, uint32_t pImageIndex);

		[[nodiscard]] VkSurfaceFormatKHR GetImageFormat() const { return m_ImageFormat; }
		[[nodiscard]] uint32_t GetImageCount() const { return m_ImageCount; }
		[[nodiscard]] uint8_t GetMaxFrameInFlight() const { return m_MaxFrameInFlight; }
		[[nodiscard]] VulkanFrameBuffer* GetFrameBufferAt(const uint32_t pIndex) const { return m_FrameBuffers[pIndex]; }
		

	private:
		void Create(uint32_t pWidth, uint32_t pHeight);
		void Destroy() const;
		VulkanContext* m_Context;
		
		VkSurfaceFormatKHR m_ImageFormat;
		uint8_t m_MaxFramesInFlight;
		VkSwapchainKHR m_Handle;
		uint32_t m_ImageCount;
		VkImage* m_Images;
		VkImageView* m_Views;

		VulkanImage* m_DepthAttachment;

		std::vector<VulkanFrameBuffer*> m_FrameBuffers;
		uint8_t m_MaxFrameInFlight = 2;
		friend class VulkanContext;
	};
	
}
