#pragma once
#include <optional>
#include <vector>

#include "Platform/Windows/Window.h"

namespace Owl::Renderer::Vulkan
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;
		std::optional<uint32_t> TransferFamily;

		[[nodiscard]] bool IsComplete() const
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value() && TransferFamily.has_value();
		}
	};

	class Device
	{
	public:
		Device(Scope<Window>& pWindow);
		~Device();

		[[nodiscard]] VkDevice GetDevice() const { return m_Device; }
		[[nodiscard]] VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		[[nodiscard]] VkQueue GetPresentQueue() const { return m_PresentQueue; }
		[[nodiscard]] VkSurfaceKHR GetSurface() const { return m_Surface; }
		[[nodiscard]] VkCommandPool GetGraphicsCommandPool() const { return m_GraphicsCommandPool; }

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(const Device&&) = delete;

		SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_PhysicalDevice); }
		uint32_t FindMemoryType(uint32_t pTypeFilter, VkMemoryPropertyFlags pProperties) const;
		QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }
		VkFormat FindSupportedFormat(
			const std::vector<VkFormat>& pCandidates, VkImageTiling pTiling, VkFormatFeatureFlags pFeatures) const;

		// Buffer Helper Functions
		void CreateBuffer(
			VkDeviceSize pSize,
			VkBufferUsageFlags pUsage,
			VkMemoryPropertyFlags pProperties,
			VkBuffer& pBuffer,
			VkDeviceMemory& pBufferMemory) const;
		VkCommandBuffer BeginSingleTimeGraphicsCommands() const;
		void EndSingleTimeGraphicsCommands(VkCommandBuffer pCommandBuffer) const;
		VkCommandBuffer BeginSingleTimeTransferCommands() const;
		void EndSingleTimeTransferCommands(VkCommandBuffer pCommandBuffer) const;
		void CopyBuffer(VkBuffer pSrcBuffer, VkBuffer pDstBuffer, VkDeviceSize pSize) const;
		void CopyBufferToImage(
			VkBuffer pBuffer, VkImage pImage, uint32_t pWidth, uint32_t pHeight) const;

		void CreateImageWithInfo(
			const VkImageCreateInfo& pImageInfo,
			VkMemoryPropertyFlags pProperties,
			VkImage& pImage,
			VkDeviceMemory& pImageMemory) const;
		VkImageView CreateImageView(VkImage pImage, VkFormat pFormat) const;

		VkPhysicalDeviceProperties Properties;

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPools();

		VkCommandBuffer BeginSingleTimeCommands(const VkCommandPool& pCommandPool) const;
		void EndSingleTimeCommands(VkCommandBuffer pCommandBuffer, const VkCommandPool& pCommandPool,
		                           const VkQueue& pQueue) const;
		bool IsDeviceSuitable(VkPhysicalDevice pDevice) const;
		[[nodiscard]] std::vector<const char*> GetRequiredExtensions() const;
		[[nodiscard]] bool CheckValidationLayerSupport() const;
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice pDevice) const;
		static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& pCreateInfo);
		void HasGflwRequiredInstanceExtensions() const;
		bool CheckDeviceExtensionSupport(VkPhysicalDevice pDevice) const;
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice pDevice) const;

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		Scope<Window>& m_Window;
		VkCommandPool m_GraphicsCommandPool;
		VkCommandPool m_TransferCommandPool;

		VkDevice m_Device;
		VkSurfaceKHR m_Surface;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkQueue m_TransferQueue;

#ifdef OWL_DEBUG
		bool m_EnableValidationLayers = true;
#else
		bool m_EnableValidationLayers = false;
#endif
		const std::vector<const char*> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
		const std::vector<const char*> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	};
}
