#pragma once
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>

#include "Device.h"
#include "SwapChain.h"

namespace Owl::Renderer::Vulkan
{
	class Renderer
	{
	public:
		Renderer(Scope<WindowOld>& pWindow, Device& pDevice);
		~Renderer();

		Device& GetDevice() { return m_Device; }

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		[[nodiscard]] VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->GetRenderPass(); }
		[[nodiscard]] float GetAspectRatio() const { return m_SwapChain->GetExtentAspectRatio(); }
		[[nodiscard]] bool IsFrameInProgress() const { return m_IsFrameProcessing; }

		[[nodiscard]] VkCommandBuffer GetCurrentCommandBuffer() const
		{
			OWL_CORE_ASSERT(m_IsFrameProcessing, "[VulkanRenderer] Cannot get buffer when frame not in progress!")
			return m_CommandBuffers[m_CurrentFrameIndex];
		}

		[[nodiscard]] int GetFrameIndex() const
		{
			OWL_CORE_ASSERT(m_IsFrameProcessing, "[VulkanRenderer] Cannot get frame index when frame not in progress!")
			return m_CurrentFrameIndex;
		}

		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer pCommandBuffer) const;
		void EndSwapChainRenderPass(VkCommandBuffer pCommandBuffer) const;

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();

		Scope<WindowOld>& m_Window;
		Device& m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentImageIndex;
		int m_CurrentFrameIndex;
		bool m_IsFrameProcessing;
	};
}
