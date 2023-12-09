#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "VulkanContext.h"
#include "Owl/Memory/Memory.h"

namespace Owl
{
	struct PhysicalDeviceRequirement
	{
		// TODO : Should be in config
		bool Graphics = true;
		bool Present = true;
		bool Compute = true;
		bool Transfer = true;
		std::vector<const char*> ExtensionNames{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		bool SamplerAnisotropy = true;
		bool DiscreteGpu = true;
	};

	struct QueueFamilyIndices
	{
		int GraphicsFamily = -1;
		int PresentFamily = -1;
		int ComputeFamily = -1;
		int TransferFamily = -1;

		bool IsValid()
		{
			// TODO : Should be in config
			return GraphicsFamily != -1 || PresentFamily != -1 || TransferFamily != -1;
		}
	};

	struct SwapchainInfo
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		uint32_t FormatCount;
		std::vector<VkSurfaceFormatKHR> Formats;
		uint32_t PresentModeCount;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(VulkanContext* pContext);
		~VulkanDevice();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagRenderer); }

		void QuerySwapchainSupport(VkPhysicalDevice pDevice,
								   SwapchainInfo& pSwapchainInfo) const;
		void DetectDepthFormat();

		VkDevice& GetLogicalDevice() { return m_LogicalDevice; }
		VkPhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }

		QueueFamilyIndices& GetQueueFamilyIndices() { return m_QueueFamilyIndices; }
		SwapchainInfo& GetSwapchainInfo() { return m_SwapchainInfo; }
		
		VkQueue& GetGraphicsQueue() { return m_Graphics; }
		VkQueue& GetPresentQueue() { return m_Present; }
		VkQueue& GetTransferQueue() { return m_Transfer; }

		VkFormat& GetDepthFormat() { return m_DepthFormat; }

	private:
		void SelectPhysicalDevice();
		bool DoPhysicalDeviceMeetRequirements(VkPhysicalDevice pDevice,
		                                      const VkPhysicalDeviceFeatures* pFeatures,
		                                      QueueFamilyIndices& pQueueFamilyIndices, SwapchainInfo& pSwapchainInfo);
		void CreateLogicalDevice();
		void GetQueues();

		VulkanContext* m_Context;
		
		VkDevice m_LogicalDevice;
		VkPhysicalDevice m_PhysicalDevice;

		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceMemoryProperties m_Memory;

		VkQueue m_Graphics;
		VkQueue m_Present;
		VkQueue m_Transfer;

		PhysicalDeviceRequirement m_PhysicalDeviceRequirement{};
		QueueFamilyIndices m_QueueFamilyIndices{};
		SwapchainInfo m_SwapchainInfo{};

		VkFormat m_DepthFormat;
	};
}
