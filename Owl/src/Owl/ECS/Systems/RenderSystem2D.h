#pragma once
#include "Owl/ECS/Ecs.h"
#include "Platform/Vulkan/Api.h"
#include "Platform/Vulkan/FrameInfo.h"
#include "Platform/Vulkan/SimplePipeline.h"

namespace Owl
{
	class RenderSystem2D : public Ecs::System
	{
	public:
		class Pipeline
		{
		public:
			Pipeline(Renderer::Vulkan::Device& pDevice, VkRenderPass pRenderPass,
			         VkDescriptorSetLayout pGlobalSetLayout);
			~Pipeline();

			Scope<Renderer::Vulkan::GraphicsPipeline>& GetPipeline() { return m_Pipeline; }
			VkPipelineLayout& GetPipelineLayout() { return m_PipelineLayout; }

		private:
			void CreatePipelineLayout(VkDescriptorSetLayout pGlobalSetLayout);
			void CreatePipeline(VkRenderPass pRenderPass);

			Renderer::Vulkan::Device& m_Device;

			Scope<Renderer::Vulkan::GraphicsPipeline> m_Pipeline;
			VkPipelineLayout m_PipelineLayout;
		};

		explicit RenderSystem2D(Ecs::World* pWorld)
			: System(pWorld)
		{
		}

		void SetPipeLine(Renderer::Vulkan::Device& pDevice, const VkRenderPass pRenderPass,
		                 VkDescriptorSetLayout pGlobalSetLayout)
		{
			m_Pipeline = CreateScope<Pipeline>(pDevice, pRenderPass, pGlobalSetLayout);
		}

		void Render(const Renderer::Vulkan::FrameInfo& pFrameInfo);

	private:
		Scope<Pipeline> m_Pipeline;
	};
}
