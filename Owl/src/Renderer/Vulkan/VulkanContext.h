#pragma once

#include <vulkan/vulkan.h>

#include "Owl/Memory/Memory.h"

namespace Owl
{
	class VulkanDevice;

	struct VulkanContext
	{
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagRenderer); }

		VkInstance Instance;
		VkAllocationCallbacks* Allocator;
#ifdef OWL_DEBUG
		VkDebugUtilsMessengerEXT DebugMessenger;
#endif
		VkSurfaceKHR Surface;

		VulkanDevice* Device;
	};
}
