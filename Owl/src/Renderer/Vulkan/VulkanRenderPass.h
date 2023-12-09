#pragma once
#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "glm/vec4.hpp"

namespace Owl
{
	enum VulkanRenderPassState
	{
		RenderPassStateReady,
		RenderPassStateRecording,
		RenderPassStateInRenderPass,
		RenderPassStateRecordingEnded,
		RenderPassStateSubmitted,
		RenderPassStateNotAllocated
	};
	
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VulkanContext* pContext, glm::vec4 pRect, float pDepth, uint32_t pStencil);
		~VulkanRenderPass();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagRenderer); }

		void SetClearColor(glm::vec4 pColor);
		
		void Begin(VulkanCommandBuffer pCommandBuffer, VkFramebuffer pFramebuffer);
		void End(VulkanCommandBuffer pCommandBuffer);
		
		VkRenderPass& GetHandle() { return m_Handle; }

	private:
		VulkanContext* m_Context;

		VkRenderPass m_Handle;

		float m_Depth;
		uint32_t m_Stencil;

		VkRenderPassBeginInfo m_Info;
		
		VulkanRenderPassState m_State;
	};
	
}
