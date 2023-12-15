#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Owl/Memory/Memory.h"

namespace Owl
{
	class VulkanDevice;
	class VulkanSwapchain;
	class VulkanRenderPass;
	class VulkanCommandBuffer;
	class VulkanFence;

	struct VulkanContext
	{
		void Initialize();
		~VulkanContext();
		
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagRenderer); }
		void operator delete(void* pBlock, const size_t pSize) { return OWL_FREE(pBlock, pSize, Owl::MemoryTagRenderer); }

		bool RecreateSwapChain();

		[[nodiscard]] int FindMemoryIndex(uint32_t pTypeFilter, VkFlags pPropertyFlags) const;

		VkInstance Instance;
		VkAllocationCallbacks* Allocator;
#ifdef OWL_DEBUG
		VkDebugUtilsMessengerEXT DebugMessenger;
#endif
		VkSurfaceKHR Surface;
		VulkanDevice* Device;
		VulkanSwapchain* Swapchain;
		VulkanRenderPass* MainRenderPass;

		std::vector<VulkanCommandBuffer*> GraphicsCommandBuffers;

		std::vector<VkSemaphore> ImageAvailableSemaphore;
		std::vector<VkSemaphore> QueueCompleteSemaphore;
		std::vector<VulkanFence*> InFlightFences;

		std::vector<VulkanFence*> ImagesInFlight;

		uint32_t FramebufferWidth;
		uint32_t FramebufferHeight;
		bool IsRecreatingSwapchain;
		bool NeedNewSwapchain;

		uint8_t CurrentFrame;
		uint32_t ImageIndex;

	private:
		void RegenerateFrameBuffers();
		void CreateCommandBuffers();
		void CreateSemaphoresAndFences();
	};
}
