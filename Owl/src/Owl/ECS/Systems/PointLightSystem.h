#pragma once
#include "Owl/ECS/Ecs.h"
#include "Platform/Vulkan/Api.h"
#include "Platform/Vulkan/FrameInfo.h"
#include "Platform/Vulkan/SimplePipeline.h"

namespace Owl
{
	class PointLightSystem : public System
	{
	public:
		class Pipeline
		{
		public:
			Pipeline(Renderer::Vulkan::Device& pDevice, VkRenderPass pRenderPass,
			         VkDescriptorSetLayout pGlobalSetLayout);
			~Pipeline();

			Pipeline(const Pipeline&) = delete;
			Pipeline& operator=(const Pipeline&) = delete;

			std::unique_ptr<Renderer::Vulkan::GraphicsPipeline>& GetPipeline() { return m_Pipeline; }
			VkPipelineLayout& GetPipelineLayout() { return m_PipelineLayout; }

		private:
			void CreatePipelineLayout(VkDescriptorSetLayout pGlobalSetLayout);
			void CreatePipeline(VkRenderPass pRenderPass);

			Renderer::Vulkan::Device& m_Device;

			std::unique_ptr<Renderer::Vulkan::GraphicsPipeline> m_Pipeline;
			VkPipelineLayout m_PipelineLayout;
		};

		explicit PointLightSystem(World* pWorld)
			: System(pWorld)
		{
		}

		void SetPipeLine(Renderer::Vulkan::Device& pDevice, const VkRenderPass pRenderPass,
		                 VkDescriptorSetLayout pGlobalSetLayout)
		{
			m_Pipeline = std::make_unique<Pipeline>(pDevice, pRenderPass, pGlobalSetLayout);
		}

		void Update(const Renderer::Vulkan::FrameInfo& pFrameInfo, Renderer::Vulkan::GlobalUbo& pGlobalUbo) const;
		void Render(const Renderer::Vulkan::FrameInfo& pFrameInfo) const;

	private:
		std::unique_ptr<Pipeline> m_Pipeline = nullptr;
	};
}
