#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "Owl/Memory/Memory.h"

namespace Owl
{
	struct PhysicalDeviceRequirement
	{
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
			return GraphicsFamily != -1 || PresentFamily != -1 || ComputeFamily != -1 || TransferFamily != -1;
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
		VulkanDevice(VkInstance pInstance, VkSurfaceKHR pSurface);
		~VulkanDevice() = default;

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagRenderer); }

	private:
		static void QuerySwapchainSupport(VkPhysicalDevice pDevice, VkSurfaceKHR pSurface,
		                                  SwapchainInfo& pSwapchainInfo);
		void SelectPhysicalDevice(VkInstance pInstance, VkSurfaceKHR pSurface);
		bool DoPhysicalDeviceMeetRequirements(VkPhysicalDevice pDevice, VkSurfaceKHR pSurface,
		                                      const VkPhysicalDeviceFeatures* pFeatures,
		                                      QueueFamilyIndices& pQueueFamilyIndices, SwapchainInfo& pSwapchainInfo);

		VkDevice m_Device;
		VkPhysicalDevice m_PhysicalDevice;

		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceMemoryProperties m_Memory;


		PhysicalDeviceRequirement m_PhysicalDeviceRequirement{};
		QueueFamilyIndices m_QueueFamilyIndices{};
		SwapchainInfo m_SwapchainInfo{};
	};
}
