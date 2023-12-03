#include "opch.h"
#include "GraphicsPipeline.h"

#include <fstream>

#include "Sprite.h"

namespace Owl::Renderer::Vulkan
{
	GraphicsPipeline::GraphicsPipeline(Device& pDevice, const std::string& pVertFilePath,
	                                   const std::string& pFragFilePath, const PipelineConfigInfo& pConfigInfo)
		: m_Device(pDevice)
	{
		CreateGraphicsPipeline(pVertFilePath, pFragFilePath, pConfigInfo);
	}

	GraphicsPipeline::~GraphicsPipeline()
	{
		vkDestroyShaderModule(m_Device.GetDevice(), m_VertShaderModule, nullptr);
		vkDestroyShaderModule(m_Device.GetDevice(), m_FragShaderModule, nullptr);
		vkDestroyPipeline(m_Device.GetDevice(), m_GrpahicsPipeline, nullptr);
	}

	void GraphicsPipeline::Bind(const VkCommandBuffer pCommandBuffer) const
	{
		vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GrpahicsPipeline);
	}

	void GraphicsPipeline::DefaultPipelineConfig(PipelineConfigInfo& pConfigInfo)
	{
		pConfigInfo.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pConfigInfo.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pConfigInfo.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		pConfigInfo.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pConfigInfo.ViewportInfo.viewportCount = 1;
		pConfigInfo.ViewportInfo.pViewports = nullptr;
		pConfigInfo.ViewportInfo.scissorCount = 1;
		pConfigInfo.ViewportInfo.pScissors = nullptr;

		pConfigInfo.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pConfigInfo.RasterizationInfo.depthClampEnable = VK_FALSE;
		pConfigInfo.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		pConfigInfo.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		pConfigInfo.RasterizationInfo.lineWidth = 1.0f;
		pConfigInfo.RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		pConfigInfo.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		pConfigInfo.RasterizationInfo.depthBiasEnable = VK_FALSE;
		pConfigInfo.RasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
		pConfigInfo.RasterizationInfo.depthBiasClamp = 0.0f; // Optional
		pConfigInfo.RasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional

		pConfigInfo.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pConfigInfo.MultisampleInfo.sampleShadingEnable = VK_FALSE;
		pConfigInfo.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pConfigInfo.MultisampleInfo.minSampleShading = 1.0f; // Optional
		pConfigInfo.MultisampleInfo.pSampleMask = nullptr; // Optional
		pConfigInfo.MultisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
		pConfigInfo.MultisampleInfo.alphaToOneEnable = VK_FALSE; // Optional

		pConfigInfo.ColorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		pConfigInfo.ColorBlendAttachment.blendEnable = VK_FALSE;
		pConfigInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		pConfigInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		pConfigInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		pConfigInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		pConfigInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		pConfigInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		pConfigInfo.ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pConfigInfo.ColorBlendInfo.logicOpEnable = VK_FALSE;
		pConfigInfo.ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
		pConfigInfo.ColorBlendInfo.attachmentCount = 1;
		pConfigInfo.ColorBlendInfo.pAttachments = &pConfigInfo.ColorBlendAttachment;
		pConfigInfo.ColorBlendInfo.blendConstants[0] = 0.0f; // Optional
		pConfigInfo.ColorBlendInfo.blendConstants[1] = 0.0f; // Optional
		pConfigInfo.ColorBlendInfo.blendConstants[2] = 0.0f; // Optional
		pConfigInfo.ColorBlendInfo.blendConstants[3] = 0.0f; // Optional

		pConfigInfo.DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pConfigInfo.DepthStencilInfo.depthTestEnable = VK_TRUE;
		pConfigInfo.DepthStencilInfo.depthWriteEnable = VK_TRUE;
		pConfigInfo.DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		pConfigInfo.DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		pConfigInfo.DepthStencilInfo.minDepthBounds = 0.0f; // Optional
		pConfigInfo.DepthStencilInfo.maxDepthBounds = 1.0f; // Optional
		pConfigInfo.DepthStencilInfo.stencilTestEnable = VK_FALSE;
		pConfigInfo.DepthStencilInfo.front = {}; // Optional
		pConfigInfo.DepthStencilInfo.back = {}; // Optional

		pConfigInfo.DynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		pConfigInfo.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pConfigInfo.DynamicStateInfo.pDynamicStates = pConfigInfo.DynamicStateEnables.data();
		pConfigInfo.DynamicStateInfo.dynamicStateCount = pConfigInfo.DynamicStateEnables.size();
		pConfigInfo.DynamicStateInfo.flags = 0;

		pConfigInfo.BindingDescriptions = Sprite::Vertex::GetBindingDescriptions();
		pConfigInfo.AttributeDescriptions = Sprite::Vertex::GetAttributeDescriptions();
	}

	void GraphicsPipeline::EnableAlphaBlending(PipelineConfigInfo& pConfigInfo)
	{
		pConfigInfo.ColorBlendAttachment.blendEnable = VK_TRUE;
		pConfigInfo.ColorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		pConfigInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		pConfigInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		pConfigInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		pConfigInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		pConfigInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		pConfigInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	}

	std::vector<char> GraphicsPipeline::ReadFile(const std::string& pFilePath)
	{
		std::ifstream file(pFilePath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("failed to open file!");

		const size_t fileSize = file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	void GraphicsPipeline::CreateGraphicsPipeline(const std::string& pVertFilePath,
	                                              const std::string& pFragFilePath,
	                                              const PipelineConfigInfo& pConfigInfo)
	{
		OWL_ASSERT(pConfigInfo.PipelineLayout != VK_NULL_HANDLE,
		           "[VulkanPipeline] Cannot create graphics pipeline, no PipelineLayout provided in configInfo!")
		OWL_ASSERT(pConfigInfo.RenderPass != VK_NULL_HANDLE,
		           "[VulkanPipeline] Cannot create graphics pipeline, no RenderPass provided in configInfo!")

		CreateShaderModule(ReadFile(pVertFilePath), &m_VertShaderModule);
		CreateShaderModule(ReadFile(pFragFilePath), &m_FragShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = m_VertShaderModule;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = m_FragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(pConfigInfo.AttributeDescriptions.
			size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(pConfigInfo.BindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = pConfigInfo.AttributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = pConfigInfo.BindingDescriptions.data();

		VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStages;
		pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
		pipelineCreateInfo.pInputAssemblyState = &pConfigInfo.InputAssemblyInfo;
		pipelineCreateInfo.pViewportState = &pConfigInfo.ViewportInfo;
		pipelineCreateInfo.pRasterizationState = &pConfigInfo.RasterizationInfo;
		pipelineCreateInfo.pMultisampleState = &pConfigInfo.MultisampleInfo;
		pipelineCreateInfo.pColorBlendState = &pConfigInfo.ColorBlendInfo;
		pipelineCreateInfo.pDepthStencilState = &pConfigInfo.DepthStencilInfo;
		pipelineCreateInfo.pDynamicState = &pConfigInfo.DynamicStateInfo;

		pipelineCreateInfo.layout = pConfigInfo.PipelineLayout;
		pipelineCreateInfo.renderPass = pConfigInfo.RenderPass;
		pipelineCreateInfo.subpass = pConfigInfo.SubPass;

		pipelineCreateInfo.basePipelineIndex = -1;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_Device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
		                              &m_GrpahicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("failed to create graphics pipeline");
	}

	void GraphicsPipeline::CreateShaderModule(const std::vector<char>& pCode, VkShaderModule* pShaderModule) const
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = pCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(pCode.data());

		if (vkCreateShaderModule(m_Device.GetDevice(), &createInfo, nullptr, pShaderModule) != VK_SUCCESS)
			throw std::runtime_error("failed to create shader module!");
	}
}
