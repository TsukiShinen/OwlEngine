#pragma once
#include "Renderer/Vulkan/VulkanContext.h"

namespace Owl
{
	struct VulkanShaderStage
	{
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagRenderer); }

		void operator delete(void* pBlock, const size_t pSize)
		{
			return OWL_FREE(pBlock, pSize, Owl::MemoryTagRenderer);
		}
		
		VkShaderModuleCreateInfo CreateInfo;
		VkShaderModule Handle;
		VkPipelineShaderStageCreateInfo ShaderStageCreateInfo;
	};
}
