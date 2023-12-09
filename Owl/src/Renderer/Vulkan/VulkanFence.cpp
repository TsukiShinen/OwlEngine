#include "VulkanFence.h"

#include <stdexcept>

#include "VulkanDevice.h"
#include "Owl/Debug/Log.h"

namespace Owl
{
	VulkanFence::VulkanFence(VulkanContext* pContext, const bool pCreateSignaled)
		: m_Context(pContext), m_IsSignaled(pCreateSignaled)
	{
		VkFenceCreateInfo fenceCreateInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
		if (m_IsSignaled) {
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		}

		if (vkCreateFence(m_Context->Device->GetLogicalDevice(), &fenceCreateInfo, m_Context->Allocator, &m_Handle) != VK_SUCCESS)
			throw std::runtime_error("[VulkanFence] Failed to create fence!");
	}

	VulkanFence::~VulkanFence()
	{
		vkDestroyFence(m_Context->Device->GetLogicalDevice(), m_Handle, m_Context->Allocator);
	}

	bool VulkanFence::Wait(const uint64_t pTimoutNanoSecond)
	{
		if (!m_IsSignaled) {
			switch (vkWaitForFences(m_Context->Device->GetLogicalDevice(), 1, &m_Handle, true, pTimoutNanoSecond)) {
			case VK_SUCCESS:
				m_IsSignaled = true;
				return true;
			case VK_TIMEOUT:
				OWL_CORE_WARN("[VulkanFence] - Timed out");
				break;
			case VK_ERROR_DEVICE_LOST:
				OWL_CORE_WARN("[VulkanFence] - VK_ERROR_DEVICE_LOST.");
				break;
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				OWL_CORE_WARN("[VulkanFence] - VK_ERROR_OUT_OF_HOST_MEMORY.");
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				OWL_CORE_WARN("[VulkanFence] - VK_ERROR_OUT_OF_DEVICE_MEMORY.");
				break;
			default:
				OWL_CORE_WARN("[VulkanFence] - An unknown error has occurred.");
				break;
			}
		} else
			return true;

		return false;
	}

	void VulkanFence::Reset()
	{
		if (m_IsSignaled) {
			if (vkResetFences(m_Context->Device->GetLogicalDevice(), 1, &m_Handle) != VK_SUCCESS)
				throw std::runtime_error("[VulkanFence] Failed to reset fences");
			m_IsSignaled = false;
		}
	}
}
