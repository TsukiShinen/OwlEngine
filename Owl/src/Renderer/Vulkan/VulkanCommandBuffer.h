#pragma once
#include "VulkanContext.h"
#include "VulkanContext.h"
#include "Owl/Memory/Memory.h"

namespace Owl
{
	enum VulkanCommandBufferState
	{
		CommandBufferStateReady,
		CommandBufferStateRecording,
		CommandBufferStateInRenderPass,
		CommandBufferStateRecordingEnded,
		CommandBufferStateSubmitted
	};

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer(VulkanContext* pVulkanContext, VkCommandPool& pPool, bool pIsPrimary);
		~VulkanCommandBuffer();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(VulkanCommandBuffer), MemoryTagRenderer); }

		void Begin(bool pIsRenderPassContinue, bool pIsSimultaneousUse, bool pIsSingUse = false);
		void End();
		void Submitted();
		void Reset();

		[[nodiscard]] VkCommandBuffer& GetHandle() { return m_Handle; }
		[[nodiscard]] VulkanCommandBufferState GetState() const { return m_State; }
		void SetState(const VulkanCommandBufferState pState) { m_State = pState; }

		static VulkanCommandBuffer* CreateAndBeginSingleTimeUse(VulkanContext* pVulkanContext, VkCommandPool& pPool);
		static void DeleteAndEndSingleTimeUse(VulkanContext* pVulkanContext, VkCommandPool& pPool,
		                                      VulkanCommandBuffer* pCommandBuffer, VkQueue pQueue);

	private:
		VulkanContext* m_Context;

		VkCommandBuffer m_Handle;
		VkCommandPool& m_Pool;

		VulkanCommandBufferState m_State;
	};
}
