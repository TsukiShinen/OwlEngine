#include "opch.h"
#include "RenderSystem2D.h"

#include "Owl/ECS/Components/TransformComponent.h"
#include "Owl/ECS/World.h"
#include "Owl/ECS/Components/SpriteComponent.h"

namespace Owl
{
	struct SimplePushConstantData
	{
		alignas(16) glm::vec4 Color{1.f, 1.f, 1.f, 1.f};
		alignas(16) glm::mat4 SpriteMatrix{1.f};
	};

	RenderSystem2D::Pipeline::Pipeline(Renderer::Vulkan::Device& pDevice, const VkRenderPass pRenderPass,
	                                   VkDescriptorSetLayout pGlobalSetLayout)
		: m_Device(pDevice)
	{
		CreatePipelineLayout(pGlobalSetLayout);
		CreatePipeline(pRenderPass);
	}

	RenderSystem2D::Pipeline::~Pipeline()
	{
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
	}

	void RenderSystem2D::Pipeline::CreatePipelineLayout(VkDescriptorSetLayout pGlobalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

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

	void RenderSystem2D::Pipeline::CreatePipeline(const VkRenderPass pRenderPass)
	{
		OWL_CORE_ASSERT(m_PipelineLayout != nullptr, "[VulkanRenderer] Cannot create pipeline before pipeline layout!")

		Renderer::Vulkan::PipelineConfigInfo pipelineConfig{};
		Renderer::Vulkan::GraphicsPipeline::DefaultPipelineConfig(pipelineConfig);
		pipelineConfig.RenderPass = pRenderPass;
		pipelineConfig.PipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Renderer::Vulkan::GraphicsPipeline>(
			m_Device, "Assets/Shaders/2D/shader.vert.spv", "Assets/Shaders/2D/shader.frag.spv",
			pipelineConfig);
	}

	void RenderSystem2D::Render(const Renderer::Vulkan::FrameInfo& pFrameInfo)
	{
		OWL_PROFILE_FUNCTION();
		OWL_CORE_ASSERT(m_Pipeline != nullptr, "[RenderSystem2D] Pipeline must be set before rendrering!")
		m_Pipeline->GetPipeline()->Bind(pFrameInfo.CommandBuffer);

		for (const auto& entity : m_Entities)
		{
			auto& transform = m_World->GetComponent<TransformComponent>(entity);
			auto& sprite = m_World->GetComponent<SpriteComponent>(entity);

			SimplePushConstantData push{};
			push.SpriteMatrix = transform.GetMatrix();
			push.Color = sprite.Color;

			vkCmdPushConstants(pFrameInfo.CommandBuffer, m_Pipeline->GetPipelineLayout(),
			                   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
			                   sizeof(SimplePushConstantData), &push);


			const VkDescriptorSet sets[2] = {
				pFrameInfo.GlobalDescriptorSet, sprite.Sprite->GetTexture()->GetDescriptorSet()
			};
			vkCmdBindDescriptorSets(pFrameInfo.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			                        m_Pipeline->GetPipelineLayout(), 0, 2, sets, 0, nullptr);

			sprite.Sprite->Bind(pFrameInfo.CommandBuffer);
			sprite.Sprite->Draw(pFrameInfo.CommandBuffer);
		}
	}
}
