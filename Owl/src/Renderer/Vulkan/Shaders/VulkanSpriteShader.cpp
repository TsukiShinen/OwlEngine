#include "VulkanSpriteShader.h"

#include "VulkanShaderUtils.h"
#include "Owl/Debug/Log.h"
#include "Owl/Math/Vector2.h"
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

		 // Pipeline creation
	    VkViewport viewport;
	    viewport.x = 0.0f;
	    viewport.y = static_cast<float>(m_Context->FramebufferHeight);
	    viewport.width = static_cast<float>(m_Context->FramebufferWidth);
	    viewport.height = -static_cast<float>(m_Context->FramebufferHeight);
	    viewport.minDepth = 0.0f;
	    viewport.maxDepth = 1.0f;

	    // Scissor
	    VkRect2D scissor;
	    scissor.offset.x = scissor.offset.y = 0;
	    scissor.extent.width = m_Context->FramebufferWidth;
	    scissor.extent.height = m_Context->FramebufferHeight;

	    // Attributes
	    uint32_t offset = 0;
		constexpr int attributeCount = 1;
	    VkVertexInputAttributeDescription attributeDescriptions[attributeCount];
	    // Position
		constexpr VkFormat formats[attributeCount] = {
	        VK_FORMAT_R32G32_SFLOAT
	    };
		constexpr uint64_t sizes[attributeCount] = {
	        sizeof(Vector2)
	    };
	    for (uint32_t i = 0; i < attributeCount; ++i) {
	        attributeDescriptions[i].binding = 0;   // binding index - should match binding desc
	        attributeDescriptions[i].location = i;  // attrib location
	        attributeDescriptions[i].format = formats[i];
	        attributeDescriptions[i].offset = offset;
	        offset += sizes[i];
	    }

	    // TODO: Desciptor set layouts.

	    // Stages
	    // NOTE: Should match the number of shader->stages.
	    VkPipelineShaderStageCreateInfo stageCreateInfos[SPRITE_SHADER_STAGE_COUNT];
	    for (uint32_t i = 0; i < SPRITE_SHADER_STAGE_COUNT; ++i) {
	        stageCreateInfos[i] = m_Stages[i].ShaderStageCreateInfo;
	        stageCreateInfos[i].sType = m_Stages[i].ShaderStageCreateInfo.sType;
	    }

	    m_Pipeline = CreateScope<VulkanPipeline>(m_Context, m_Context->MainRenderPass, attributeCount, attributeDescriptions, 0, nullptr,  SPRITE_SHADER_STAGE_COUNT, stageCreateInfos, viewport, scissor, false);
	}

	VulkanSpriteShader::~VulkanSpriteShader()
	{
		for (const auto& stage : m_Stages)
			vkDestroyShaderModule(m_Context->Device->GetLogicalDevice(), stage.Handle, m_Context->Allocator);
	}

	void VulkanSpriteShader::Use()
	{
	}
}
