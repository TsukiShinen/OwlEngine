#pragma once

#include <vulkan/vulkan.h>

#include "Owl/Memory/Memory.h"

namespace Owl
{
	class VulkanDevice;
	class VulkanSwapchain;
	class VulkanRenderPass;

	struct VulkanContext
	{
		void Initialize();
		~VulkanContext();
		
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagRenderer); }
		
		int FindMemoryIndex(uint32_t pTypeFilter, VkFlags pPropertyFlags) const;

		VkInstance Instance;
		VkAllocationCallbacks* Allocator;
#ifdef OWL_DEBUG
		VkDebugUtilsMessengerEXT DebugMessenger;
#endif
		VkSurfaceKHR Surface;
		VulkanDevice* Device;
		VulkanSwapchain* Swapchain;
		VulkanRenderPass* MainRenderPass;

		uint32_t FramebufferWidth;
		uint32_t FramebufferHeight;

		uint8_t CurrentFrame;
	};
}
