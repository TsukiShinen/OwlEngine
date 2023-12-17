#pragma once
#include "VulkanShaderStage.h"
#include "Renderer/Vulkan/VulkanContext.h"

namespace Owl
{
	class VulkanShaderUtils
	{
	public:
		static bool CreateShaderModule(const VulkanContext* pContext, const char* pName, const char* pTypeString,
		                               VkShaderStageFlagBits pFlags, VulkanShaderStage* pStage);
	};
}
