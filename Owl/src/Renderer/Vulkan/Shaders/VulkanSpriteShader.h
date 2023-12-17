#pragma once
#include "VulkanShaderStage.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanPipeline.h"

#define SPRITE_SHADER_STAGE_COUNT 2 // Vertex and fragment

namespace Owl
{
	class VulkanSpriteShader
	{
	public:
		VulkanSpriteShader(VulkanContext* pContext);
		~VulkanSpriteShader();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagRenderer); }

		void operator delete(void* pBlock, const size_t pSize)
		{
			return OWL_FREE(pBlock, pSize, Owl::MemoryTagRenderer);
		}

		void Use();

	private:
		VulkanContext* m_Context;

		VulkanShaderStage m_Stages[SPRITE_SHADER_STAGE_COUNT];
		VulkanPipeline m_Pipeline;
	};
}
