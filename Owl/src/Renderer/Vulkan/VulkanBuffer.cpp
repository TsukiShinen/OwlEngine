#include "VulkanBuffer.h"

#include <stdexcept>

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "Owl/Debug/Log.h"

namespace Owl
{
	VulkanBuffer::VulkanBuffer(VulkanContext* pContext, const uint64_t pSize, const VkFlags pUsage,
	                           const uint32_t pMemoryPropertyFlags)
			: m_Context(pContext), m_TotalSize(pSize), m_Usage(pUsage), m_IsLocked(false), m_MemoryPropertyFlags(pMemoryPropertyFlags)
	{
		VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
		bufferInfo.size = m_TotalSize;
		bufferInfo.usage = m_Usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // NOTE: Only used in one queue.

		if(vkCreateBuffer(m_Context->Device->GetLogicalDevice(), &bufferInfo, m_Context->Allocator, &m_Handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create buffer");

		// Gather memory requirements.
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(m_Context->Device->GetLogicalDevice(), m_Handle, &requirements);
		m_MemoryIndex = m_Context->FindMemoryIndex(requirements.memoryTypeBits, m_MemoryPropertyFlags);
		if (m_MemoryIndex == -1) 
			throw std::runtime_error("Unable to create vulkan buffer because the required memory type index was not found.");

		// Allocate memory info
		VkMemoryAllocateInfo allocateInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
		allocateInfo.allocationSize = requirements.size;
		allocateInfo.memoryTypeIndex = static_cast<uint32_t>(m_MemoryIndex);

		// Allocate the memory.
		if (vkAllocateMemory(m_Context->Device->GetLogicalDevice(), &allocateInfo, m_Context->Allocator, &m_Memory) != VK_SUCCESS)
			throw std::runtime_error("Unable to create vulkan buffer because the required memory allocation failed.");
	}

	VulkanBuffer::~VulkanBuffer()
	{
		if (m_Memory)
			vkFreeMemory(m_Context->Device->GetLogicalDevice(), m_Memory, m_Context->Allocator);
		if (m_Handle)
			vkDestroyBuffer(m_Context->Device->GetLogicalDevice(), m_Handle, m_Context->Allocator);
	}

	bool VulkanBuffer::Resize(uint64_t pNewSize, VkQueue pQueue, VkCommandPool pPool)
	{
		 // Create new buffer.
	    VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
	    bufferInfo.size = pNewSize;
	    bufferInfo.usage = m_Usage;
	    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // NOTE: Only used in one queue.

	    VkBuffer newBuffer;
	    if (vkCreateBuffer(m_Context->Device->GetLogicalDevice(), &bufferInfo, m_Context->Allocator, &newBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to create buffer for resize");

	    // Gather memory requirements.
	    VkMemoryRequirements requirements;
	    vkGetBufferMemoryRequirements(m_Context->Device->GetLogicalDevice(), newBuffer, &requirements);

	    // Allocate memory info
	    VkMemoryAllocateInfo allocateInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
	    allocateInfo.allocationSize = requirements.size;
	    allocateInfo.memoryTypeIndex = static_cast<uint64_t>(m_MemoryIndex);

	    // Allocate the memory.
	    VkDeviceMemory newMemory;
		 if (vkAllocateMemory(m_Context->Device->GetLogicalDevice(), &allocateInfo, m_Context->Allocator, &newMemory) != VK_SUCCESS) 
			throw std::runtime_error("Unable to create vulkan buffer for resize because the required memory allocation failed.");

	    // Copy over the data
	    CopyTo(m_Context, pPool, nullptr, pQueue, m_Handle, 0, newBuffer, 0, m_TotalSize);

	    // Make sure anything potentially using these is finished.
	    vkDeviceWaitIdle(m_Context->Device->GetLogicalDevice());

	    // Destroy the old
		if (m_Memory)
			vkFreeMemory(m_Context->Device->GetLogicalDevice(), m_Memory, m_Context->Allocator);
		if (m_Handle)
			vkDestroyBuffer(m_Context->Device->GetLogicalDevice(), m_Handle, m_Context->Allocator);

	    // Set new properties
	    m_TotalSize = pNewSize;
	    m_Memory = newMemory;
	    m_Handle = newBuffer;

	    return true;
	}

	void VulkanBuffer::Bind(const uint64_t pOffset) const
	{
		if (vkBindBufferMemory(m_Context->Device->GetLogicalDevice(), m_Handle, m_Memory, pOffset) != VK_SUCCESS)
			throw std::runtime_error("Unable to create vulkan buffer because the required memory allocation failed.");
	}

	void* VulkanBuffer::LockMemory(const uint64_t pOffset, const uint64_t pSize, const VkFlags pFlags) const
	{
		void* data;
		vkMapMemory(m_Context->Device->GetLogicalDevice(), m_Memory, pOffset, pSize, pFlags, &data);
		return data;
	}

	void VulkanBuffer::UnlockMemory() const
	{
		vkUnmapMemory(m_Context->Device->GetLogicalDevice(), m_Memory);
	}

	void VulkanBuffer::LoadData(const uint64_t pOffset, const uint64_t pSize, const VkFlags pFlags, const void* pData) const
	{
		void* dataPtr = LockMemory(pOffset, pSize, pFlags);
		OWL_COPY_MEMORY(dataPtr, pData, pSize);
		UnlockMemory();
	}

	void VulkanBuffer::CopyTo(VulkanContext* pContext, VkCommandPool pPool, VkFence pFence, const VkQueue pQueue, const VkBuffer pSource,
	                          const uint64_t pSourceOffset, const VkBuffer pDest, const uint64_t pDestOffset, const uint64_t pSize)
	{
		vkQueueWaitIdle(pQueue);
		// Create a one-time-use command buffer.
		VulkanCommandBuffer tempCommandBuffer = VulkanCommandBuffer::CreateAndBeginSingleTimeUse(pContext, pPool);

		// Prepare the copy command and add it to the command buffer.
		VkBufferCopy copyRegion;
		copyRegion.srcOffset = pSourceOffset;
		copyRegion.dstOffset = pDestOffset;
		copyRegion.size = pSize;

		vkCmdCopyBuffer(tempCommandBuffer.GetHandle(), pSource, pDest, 1, &copyRegion);

		// Submit the buffer for execution and wait for it to complete.
		VulkanCommandBuffer::DeleteAndEndSingleTimeUse(pContext, pPool, &tempCommandBuffer, pQueue);
	}
}
