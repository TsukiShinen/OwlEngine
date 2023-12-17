#pragma once
#include "Renderer/Vulkan/VulkanContext.h"

namespace Owl
{
	struct VulkanShaderStage
	{
		VkShaderModuleCreateInfo CreateInfo;
		VkShaderModule Handle;
		VkPipelineShaderStageCreateInfo ShaderStageCreateInfo;
	};
}
