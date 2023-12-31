﻿#pragma once
#include "Renderer/Vulkan/VulkanContext.h"

namespace Owl
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline(VulkanContext* pContext, VulkanRenderPass* pRenderPass, uint32_t pAttributesCount, VkVertexInputAttributeDescription* pAttributes,
			uint32_t pDescriptorSetLayoutCount, VkDescriptorSetLayout* pDescriptorSetLayout, uint32_t pStageCount, VkPipelineShaderStageCreateInfo* pStages,
			VkViewport pViewport, VkRect2D pScissor, bool pIsWireframe);
		~VulkanPipeline();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagRenderer); }

		void operator delete(void* pBlock, const size_t pSize)
		{
			return OWL_FREE(pBlock, pSize, Owl::MemoryTagRenderer);
		}

		void Bind(VulkanCommandBuffer* pCommandBuffer, VkPipelineBindPoint pBindPoint) const;

	private:
		VulkanContext* m_Context;
		
		VkPipeline m_Handle;
		VkPipelineLayout m_PipelineLayout;
	};
}
