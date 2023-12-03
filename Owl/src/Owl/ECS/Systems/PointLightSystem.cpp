#include "opch.h"
#include "PointLightSystem.h"

#include "Owl/ECS/World.h"
#include "Owl/ECS/Components/PointLightComponent.h"
#include "Owl/ECS/Components/TransformComponent.h"

namespace Owl
{
	struct PointLightPushConstant
	{
		alignas(16) glm::vec3 Position{};
		alignas(16) glm::vec4 Color{};
		alignas(16) float Radius{};
	};

	PointLightSystem::Pipeline::Pipeline(Renderer::Vulkan::Device& pDevice, const VkRenderPass pRenderPass,
	                                     VkDescriptorSetLayout pGlobalSetLayout)
		: m_Device(pDevice)
	{
		CreatePipelineLayout(pGlobalSetLayout);
		CreatePipeline(pRenderPass);
	}

	PointLightSystem::Pipeline::~Pipeline()
	{
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
	}

	void PointLightSystem::Pipeline::CreatePipelineLayout(VkDescriptorSetLayout pGlobalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstant);

		const std::vector descriptorSetLayouts{pGlobalSetLayout};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout!");
	}

	void PointLightSystem::Pipeline::CreatePipeline(const VkRenderPass pRenderPass)
	{
		OWL_CORE_ASSERT(m_PipelineLayout != nullptr, "[VulkanRenderer] Cannot create pipeline before pipeline layout!")

		Renderer::Vulkan::PipelineConfigInfo pipelineConfig{};
		Renderer::Vulkan::GraphicsPipeline::DefaultPipelineConfig(pipelineConfig);
		Renderer::Vulkan::GraphicsPipeline::EnableAlphaBlending(pipelineConfig);
		pipelineConfig.BindingDescriptions.clear();
		pipelineConfig.AttributeDescriptions.clear();
		pipelineConfig.RenderPass = pRenderPass;
		pipelineConfig.PipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Renderer::Vulkan::GraphicsPipeline>(
			m_Device, "Shaders/pointLight.vert.spv", "Shaders/pointLight.frag.spv",
			pipelineConfig);
	}

	void PointLightSystem::Update(const Renderer::Vulkan::FrameInfo& pFrameInfo,
	                              Renderer::Vulkan::GlobalUbo& pGlobalUbo) const
	{
		int lightIndex = 0;

		for (const auto& entity : m_Entities)
		{
			const auto& transform = m_World->GetComponent<TransformComponent>(entity);
			auto& pointLight = m_World->GetComponent<PointLightComponent>(entity);

			pGlobalUbo.PointLight[lightIndex].Position = transform.Position;
			pGlobalUbo.PointLight[lightIndex].Color = glm::vec4(pointLight.Color, pointLight.LightIntensity);
			lightIndex += 1;
		}
		pGlobalUbo.NumberLight = lightIndex;
	}

	void PointLightSystem::Render(const Renderer::Vulkan::FrameInfo& pFrameInfo) const
	{
		OWL_CORE_ASSERT(m_Pipeline != nullptr, "[RenderSystem] Pipeline must be set before rendrering!")
		m_Pipeline->GetPipeline()->Bind(pFrameInfo.CommandBuffer);

		vkCmdBindDescriptorSets(pFrameInfo.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		                        m_Pipeline->GetPipelineLayout(), 0, 1, &pFrameInfo.GlobalDescriptorSet, 0, nullptr);

		for (const auto& entity : m_Entities)
		{
			const auto& transform = m_World->GetComponent<TransformComponent>(entity);
			auto& pointLight = m_World->GetComponent<PointLightComponent>(entity);

			PointLightPushConstant push{};
			push.Position = transform.Position;
			push.Color = glm::vec4(pointLight.Color, pointLight.LightIntensity);
			push.Radius = transform.Scale.x;

			vkCmdPushConstants(pFrameInfo.CommandBuffer, m_Pipeline->GetPipelineLayout(),
			                   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
			                   sizeof(PointLightPushConstant), &push);

			vkCmdDraw(pFrameInfo.CommandBuffer, 6, 1, 0, 0);
		}
	}
}
