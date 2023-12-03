#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "Device.h"

namespace Owl::Renderer::Vulkan
{
	class SwapChain
	{
	public:
		static constexpr int k_MaxFramesInFlight = 3;

		SwapChain(Device& pDevice, VkExtent2D pWindowExtent);
		SwapChain(Device& pDevice, VkExtent2D pWindowExtent,
		          const std::shared_ptr<SwapChain>& pPrevious);
		~SwapChain();

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		[[nodiscard]] VkFramebuffer GetFrameBuffer(const int pIndex) const { return m_SwapChainFramebuffers[pIndex]; }
		[[nodiscard]] VkRenderPass GetRenderPass() const { return m_RenderPass; }
		[[nodiscard]] VkImageView GetImageView(const int pIndex) const { return m_SwapChainImageViews[pIndex]; }
		[[nodiscard]] size_t GetImageCount() const { return m_SwapChainImages.size(); }
		[[nodiscard]] VkFormat GetSwapChainImageFormat() const { return m_SwapChainImageFormat; }
		[[nodiscard]] VkExtent2D GetSwapChainExtent() const { return m_SwapChainExtent; }
		[[nodiscard]] uint32_t GetWidth() const { return m_SwapChainExtent.width; }
		[[nodiscard]] uint32_t GetHeight() const { return m_SwapChainExtent.height; }

		[[nodiscard]] float GetExtentAspectRatio() const
		{
			return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height);
		}

		VkFormat FindDepthFormat();

		VkResult AcquireNextImage(uint32_t* pImageIndex) const;
		VkResult SubmitCommandBuffers(const VkCommandBuffer* pBuffer, const uint32_t* pImageIndex);

		bool CompareSwapFormats(const SwapChain& pSwapChain) const
		{
			return pSwapChain.m_SwapChainDepthFormat == m_SwapChainDepthFormat &&
				pSwapChain.m_SwapChainImageFormat == m_SwapChainImageFormat;
		}

	private:
		void Initialize();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateRenderPass();
		void CreateFrameBuffers();
		void CreateSyncObject();

		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& pAvailableFormats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& pAvailablePresentMode);
		[[nodiscard]] VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& pCapabilities) const;

		VkFormat m_SwapChainImageFormat;
		VkFormat m_SwapChainDepthFormat;
		VkExtent2D m_SwapChainExtent;

		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
		VkRenderPass m_RenderPass;

		std::vector<VkImage> m_DepthImages;
		std::vector<VkDeviceMemory> m_DepthImageMemories;
		std::vector<VkImageView> m_DepthImageViews;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		Device& m_Device;
		VkExtent2D m_WindowExtent;

		VkSwapchainKHR m_SwapChain;
		std::shared_ptr<SwapChain> m_OldSwapChain;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		std::vector<VkFence> m_ImagesInFlight;
		size_t m_CurrentFrame = 0;
	};
}
