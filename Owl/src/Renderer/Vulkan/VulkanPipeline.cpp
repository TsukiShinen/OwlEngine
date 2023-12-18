#include "VulkanPipeline.h"

#include <stdexcept>

#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "Owl/Debug/Log.h"
#include "Owl/Math/Vertex2D.h"

namespace Owl
{
	VulkanPipeline::VulkanPipeline(VulkanContext* pContext, VulkanRenderPass* pRenderPass, uint32_t pAttributesCount,
		VkVertexInputAttributeDescription* pAttributes, uint32_t pDescriptorSetLayoutCount,
		VkDescriptorSetLayout* pDescriptorSetLayout, uint32_t pStageCount, VkPipelineShaderStageCreateInfo* pStages,
		VkViewport pViewport, VkRect2D pScissor, bool pIsWireframe)
			: m_Context(pContext)
	{
		 // Viewport state
	    VkPipelineViewportStateCreateInfo viewportState = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
	    viewportState.viewportCount = 1;
	    viewportState.pViewports = &pViewport;
	    viewportState.scissorCount = 1;
	    viewportState.pScissors = &pScissor;

	    // Rasterizer
	    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
	    rasterizerCreateInfo.depthClampEnable = VK_FALSE;
	    rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	    rasterizerCreateInfo.polygonMode = pIsWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	    rasterizerCreateInfo.lineWidth = 1.0f;
	    rasterizerCreateInfo.cullMode = VK_CULL_MODE_NONE;
	    rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	    rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
	    rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
	    rasterizerCreateInfo.depthBiasClamp = 0.0f;
	    rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

	    // Multisampling.
	    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
	    multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
	    multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	    multisamplingCreateInfo.minSampleShading = 1.0f;
	    multisamplingCreateInfo.pSampleMask = nullptr;
	    multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
	    multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

	    // Depth and stencil testing.
	    VkPipelineDepthStencilStateCreateInfo depthStencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
	    depthStencil.depthTestEnable = VK_TRUE;
	    depthStencil.depthWriteEnable = VK_TRUE;
	    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	    depthStencil.depthBoundsTestEnable = VK_FALSE;
	    depthStencil.stencilTestEnable = VK_FALSE;

	    VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
	    colorBlendAttachmentState.blendEnable = VK_TRUE;
	    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
	                                                  VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
	    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	    colorBlendStateCreateInfo.attachmentCount = 1;
	    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

		 constexpr uint32_t dynamicStateCount = 3;
	    VkDynamicState dynamicStates[dynamicStateCount] = {
	        VK_DYNAMIC_STATE_VIEWPORT,
	        VK_DYNAMIC_STATE_SCISSOR,
	        VK_DYNAMIC_STATE_LINE_WIDTH};

	    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
	    dynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
	    dynamicStateCreateInfo.pDynamicStates = dynamicStates;

	    // Vertex input
	    VkVertexInputBindingDescription bindingDescription;
	    bindingDescription.binding = 0;  // Binding index
	    bindingDescription.stride = sizeof(Vertex2D);
	    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;  // Move to next data entry for each vertex.

	    // Attributes
	    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
	    vertexInputInfo.vertexBindingDescriptionCount = 1;
	    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	    vertexInputInfo.vertexAttributeDescriptionCount = pAttributesCount;
	    vertexInputInfo.pVertexAttributeDescriptions = pAttributes;

	    // Input assembly
	    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
	    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	    inputAssembly.primitiveRestartEnable = VK_FALSE;

	    // Pipeline layout
	    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

	    // Descriptor set layouts
	    pipelineLayoutCreateInfo.setLayoutCount = pDescriptorSetLayoutCount;
	    pipelineLayoutCreateInfo.pSetLayouts = pDescriptorSetLayout;

	    // Create the pipeline layout.
	    if (vkCreatePipelineLayout( m_Context->Device->GetLogicalDevice(), &pipelineLayoutCreateInfo, m_Context->Allocator, &m_PipelineLayout) != VK_SUCCESS)
	    	throw std::runtime_error("Failed to create pipeline layout!");

	    // Pipeline create
	    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
	    pipelineCreateInfo.stageCount = pStageCount;
	    pipelineCreateInfo.pStages = pStages;
	    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
	    pipelineCreateInfo.pInputAssemblyState = &inputAssembly;

	    pipelineCreateInfo.pViewportState = &viewportState;
	    pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
	    pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
	    pipelineCreateInfo.pDepthStencilState = &depthStencil;
	    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
	    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
	    pipelineCreateInfo.pTessellationState = nullptr;

	    pipelineCreateInfo.layout = m_PipelineLayout;

	    pipelineCreateInfo.renderPass = pRenderPass->GetHandle();
	    pipelineCreateInfo.subpass = 0;
	    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	    pipelineCreateInfo.basePipelineIndex = -1;

		 if (vkCreateGraphicsPipelines(m_Context->Device->GetLogicalDevice(), nullptr, 1, &pipelineCreateInfo, m_Context->Allocator, &m_Handle) != VK_SUCCESS) 
	    	throw std::runtime_error("Failed to create graphics pipeline!");

		OWL_CORE_INFO("[VulkanPipeline] Graphics pipeline created!");
	}

	VulkanPipeline::~VulkanPipeline()
	{
		if (m_Handle)
			vkDestroyPipeline(m_Context->Device->GetLogicalDevice(), m_Handle, m_Context->Allocator);
		if (m_PipelineLayout)
			vkDestroyPipelineLayout(m_Context->Device->GetLogicalDevice(), m_PipelineLayout, m_Context->Allocator);
	}

	void VulkanPipeline::Bind(VulkanCommandBuffer* pCommandBuffer, const VkPipelineBindPoint pBindPoint) const
	{
		vkCmdBindPipeline(pCommandBuffer->GetHandle(), pBindPoint, m_Handle);
	}
}
