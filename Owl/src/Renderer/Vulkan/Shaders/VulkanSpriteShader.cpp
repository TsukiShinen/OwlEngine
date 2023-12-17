#include "VulkanSpriteShader.h"

#include "VulkanShaderUtils.h"
#include "Owl/Debug/Log.h"
#include "Renderer/Vulkan/VulkanDevice.h"

#define BUILTIN_SHADER_NAME_SPRITE "Builtin.SpriteShader"

namespace Owl
{
	VulkanSpriteShader::VulkanSpriteShader(VulkanContext* pContext)
		: m_Context(pContext)
	{
		for (auto& stage : m_Stages)
			stage = VulkanShaderStage();
		char stageTypesStrings[SPRITE_SHADER_STAGE_COUNT][5] = {"vert", "frag"};
		constexpr VkShaderStageFlagBits stageTypes[SPRITE_SHADER_STAGE_COUNT] = {
			VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT
		};

		for (uint32_t i = 0; i < SPRITE_SHADER_STAGE_COUNT; ++i)
		{
			if (!VulkanShaderUtils::CreateShaderModule(m_Context, BUILTIN_SHADER_NAME_SPRITE, stageTypesStrings[i],
			                                           stageTypes[i], &m_Stages[i]))
			{
				OWL_CORE_CRITICAL("[VulkanSpriteShader] Unable to create %s shader module for '%s'.",
				                  stageTypesStrings[i], BUILTIN_SHADER_NAME_SPRITE);
				return;
			}
		}

		// TODO: descriptors
	}

	VulkanSpriteShader::~VulkanSpriteShader()
	{
		for (auto& stage : m_Stages)
			vkDestroyShaderModule(m_Context->Device->GetLogicalDevice(), stage.Handle, m_Context->Allocator);
	}

	void VulkanSpriteShader::Use()
	{
	}
}
