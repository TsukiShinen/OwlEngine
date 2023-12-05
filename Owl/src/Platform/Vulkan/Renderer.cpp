#include "opch.h"
#include "Renderer.h"

namespace Owl::Renderer::Vulkan
{
	Renderer::Renderer(Scope<WindowOld>& pWindow, Device& pDevice)
		: m_Window(pWindow), m_Device(pDevice)
	{
		RecreateSwapChain();
		CreateCommandBuffers();
		m_IsFrameProcessing = false;
		m_CurrentFrameIndex = 0;
	}

	Renderer::~Renderer()
	{
		OWL_PROFILE_FUNCTION();
		FreeCommandBuffers();
	}

	VkCommandBuffer Renderer::BeginFrame()
	{
		OWL_CORE_ASSERT(!m_IsFrameProcessing, "[VulkanRenderer] Can't call BeginFrame while already in progress!")
		const auto result = m_SwapChain->AcquireNextImage(&m_CurrentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("failed to acquire swap chain image!");

		m_IsFrameProcessing = true;

		const auto commandBuffer = GetCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");

		return commandBuffer;
	}

	void Renderer::EndFrame()
	{
		OWL_CORE_ASSERT(m_IsFrameProcessing, "[VulkanRenderer] Can't call EndFrame while frame is not in progress!")
		const auto commandBuffer = GetCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer!");

		const auto result = m_SwapChain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window->WasWindowResized())
		{
			m_Window->ResetWindowResizeFlags();
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("failed to acquire swap chain image!");

		m_IsFrameProcessing = false;
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % SwapChain::k_MaxFramesInFlight;
	}

	void Renderer::BeginSwapChainRenderPass(const VkCommandBuffer pCommandBuffer) const
	{
		OWL_CORE_ASSERT(m_IsFrameProcessing,
		                "[VulkanRenderer] Can't call BeginSwapChainRenderPass while frame is not in progress!")
		OWL_CORE_ASSERT(pCommandBuffer == GetCurrentCommandBuffer(),
		                "[VulkanRenderer] Can't beging RendrePass on command buffer fram a different frame!")
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_SwapChain->GetFrameBuffer(m_CurrentImageIndex);

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {{0.f, 0.f, 0.f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(pCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_SwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		const VkRect2D scissor{{0, 0}, m_SwapChain->GetSwapChainExtent()};
		vkCmdSetViewport(pCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(pCommandBuffer, 0, 1, &scissor);
	}

	void Renderer::EndSwapChainRenderPass(const VkCommandBuffer pCommandBuffer) const
	{
		OWL_CORE_ASSERT(m_IsFrameProcessing,
		                "[VulkanRenderer] Can't call EndSwapChainRenderPass while frame is not in progress!")
		OWL_CORE_ASSERT(pCommandBuffer == GetCurrentCommandBuffer(),
		                "[VulkanRenderer] Can't end RendrePass on command buffer fram a different frame!")
		vkCmdEndRenderPass(pCommandBuffer);
	}

	void Renderer::CreateCommandBuffers()
	{
		OWL_PROFILE_FUNCTION();
		m_CommandBuffers.resize(SwapChain::k_MaxFramesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_Device.GetGraphicsCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device.GetDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate command buffers!");
	}

	void Renderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_Device.GetDevice(), m_Device.GetGraphicsCommandPool(), m_CommandBuffers.size(),
		                     m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	void Renderer::RecreateSwapChain()
	{
		OWL_PROFILE_FUNCTION();
		auto extent = m_Window->GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_Window->GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_Device.GetDevice());

		if (m_SwapChain == nullptr)
			m_SwapChain = std::make_unique<SwapChain>(m_Device, extent);
		else
		{
			std::shared_ptr oldSwapChain = std::move(m_SwapChain);
			m_SwapChain = std::make_unique<SwapChain>(m_Device, extent, oldSwapChain);

			if (!oldSwapChain->CompareSwapFormats(*m_SwapChain))
				throw std::runtime_error("SwapChain image(or depth) format has changed!");
		}
	}
}
