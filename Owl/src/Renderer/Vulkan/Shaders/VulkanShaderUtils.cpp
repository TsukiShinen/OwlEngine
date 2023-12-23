#include "VulkanShaderUtils.h"

#include <format>

#include "Owl/Platform/FilesSystem.h"
#include "Owl/Platform/Window.h"
#include "Renderer/Vulkan/VulkanDevice.h"

namespace Owl
{
	bool VulkanShaderUtils::CreateShaderModule(const VulkanContext* pContext, const char* pName,
	                                           const char* pTypeString,
	                                           const VkShaderStageFlagBits pFlags, VulkanShaderStage* pStage)
	{
		const auto filePath = std::format("{0}/Assets/Shaders/{1}.{2}.spv", Window::GetExecutablePath(), pName,
		                                  pTypeString);

		pStage->CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

		File shaderFile;
		if (!FilesSystem::TryOpen(filePath.c_str(), FileModeRead, true, shaderFile))
		{
			OWL_CORE_ERROR("[VulkanShaderUtils] Unable to open shader module: %s.", filePath);
			return false;
		}

		uint64_t size = 0;
		char* fileBuffer = nullptr;
		if (!FilesSystem::TryReadAllBytes(shaderFile, &fileBuffer, &size))
		{
			OWL_CORE_ERROR("[VulkanShaderUtils] Unable to binary read shader module: %s.", filePath);
			return false;
		}

		pStage->CreateInfo.codeSize = size;
		pStage->CreateInfo.pCode = reinterpret_cast<const uint32_t*>(fileBuffer);

		FilesSystem::Close(shaderFile);

		if (vkCreateShaderModule(pContext->Device->GetLogicalDevice(), &pStage->CreateInfo, pContext->Allocator,
		                         &pStage->Handle) != VK_SUCCESS)
			throw std::runtime_error("[VulkanShaderUtils] Failed to vkCreateShaderModule!");

		pStage->ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pStage->ShaderStageCreateInfo.stage = pFlags;
		pStage->ShaderStageCreateInfo.module = pStage->Handle;
		pStage->ShaderStageCreateInfo.pName = "main";

		if (fileBuffer)
		{
			OWL_FREE(fileBuffer, size, MemoryTagUnknown);
			fileBuffer = nullptr;
		}

		return true;
	}
}
