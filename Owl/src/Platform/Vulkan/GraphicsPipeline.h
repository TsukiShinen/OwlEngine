#pragma once

#include <string>
#include <vector>

#include "Device.h"

namespace Owl::Renderer::Vulkan
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo() = default;

		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> BindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;
		VkPipelineViewportStateCreateInfo ViewportInfo;
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo RasterizationInfo;
		VkPipelineMultisampleStateCreateInfo MultisampleInfo;
		VkPipelineColorBlendAttachmentState ColorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
		std::vector<VkDynamicState> DynamicStateEnables;
		VkPipelineDynamicStateCreateInfo DynamicStateInfo;
		VkPipelineLayout PipelineLayout = nullptr;
		VkRenderPass RenderPass = nullptr;
		uint32_t SubPass = 0;
	};

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(Device& pDevice, const std::string& pVertFilePath, const std::string& pFragFilePath,
		                 const PipelineConfigInfo& pConfigInfo);

		~GraphicsPipeline();

		GraphicsPipeline(const GraphicsPipeline&) = delete;
		GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

		void Bind(VkCommandBuffer pCommandBuffer) const;

		static void DefaultPipelineConfig(PipelineConfigInfo& pConfigInfo);
		static void EnableAlphaBlending(PipelineConfigInfo& pConfigInfo);

	private:
		static std::vector<char> ReadFile(const std::string& pFilePath);

		void CreateGraphicsPipeline(const std::string& pVertFilePath,
		                            const std::string& pFragFilePath, const PipelineConfigInfo& pConfigInfo);

		void CreateShaderModule(const std::vector<char>& pCode, VkShaderModule* pShaderModule) const;

		Device& m_Device;
		VkPipeline m_GrpahicsPipeline;
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
	};
}
