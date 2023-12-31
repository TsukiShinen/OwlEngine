﻿#pragma once
#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "Owl/Math/Vector4.h"

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
		VulkanRenderPass(VulkanContext* pContext, Vector4 pRect);
		~VulkanRenderPass();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagRenderer); }

		void operator delete(void* pBlock, const size_t pSize)
		{
			return OWL_FREE(pBlock, pSize, Owl::MemoryTagRenderer);
		}

		void SetClearColor(Vector4 pColor, float pDepth = 1, uint32_t pStencil = 0);

		void Begin(VulkanCommandBuffer* pCommandBuffer, VkFramebuffer pFramebuffer);
		void End(VulkanCommandBuffer* pCommandBuffer);

		void SetWidth(const uint32_t pWidth) { m_Info.renderArea.extent.width = pWidth; }
		void SetHeight(const uint32_t pHeight) { m_Info.renderArea.extent.height = pHeight; }

		VkRenderPass& GetHandle() { return m_Handle; }

	private:
		VulkanContext* m_Context;

		VkRenderPass m_Handle;

		VkRenderPassBeginInfo m_Info;
		VkClearValue m_ClearValues[2];

		VulkanRenderPassState m_State;
	};
}
