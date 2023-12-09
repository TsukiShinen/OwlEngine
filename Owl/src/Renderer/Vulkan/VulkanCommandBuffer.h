#pragma once
#include "VulkanContext.h"

namespace Owl
{
	enum VulkanCommandBufferState
	{
		CommandBufferStateReady,
		CommandBufferStateRecording,
		CommandBufferStateInRenderPass,
		CommandBufferStateRecordingEnded,
		CommandBufferStateSubmitted,
		CommandBufferStateNotAllocated
	};
	
	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer() = default;
		
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagRenderer); }

		[[nodiscard]] VkCommandBuffer GetHandle() const { return m_Handle; }
		[[nodiscard]] VulkanCommandBufferState GetState() const { return m_State; }
		void SetState(const VulkanCommandBufferState pState) { m_State = pState; }
		
	private:
		VkCommandBuffer m_Handle;
		VulkanCommandBufferState m_State;
	};

}
