#pragma once
#include "Renderer/Vulkan/VulkanContext.h"

namespace Owl
{
	struct VulkanPipeline
	{
		VkPipeline Handle;
		VkPipelineLayout PipelineLayout;
	};
}
