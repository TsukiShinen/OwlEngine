#include "opch.h"
#include "Buffer.h"

namespace Owl::Renderer::Vulkan
{
	Buffer::Buffer(Device& pDevice, const VkDeviceSize pInstanceSize, const uint32_t pInstanceCount,
	               const VkBufferUsageFlags pUsageFlags, const VkMemoryPropertyFlags pMemoryPropertyFlags,
	               const VkDeviceSize pMinOffsetAlignment)
		: m_Device(pDevice), m_InstanceCount(pInstanceCount), m_InstanceSize(pInstanceSize), m_UsageFlags(pUsageFlags),
		  m_MemoryPropertyFlags(pMemoryPropertyFlags)
	{
		m_AlignmentSize = GetAlignment(pInstanceSize, pMinOffsetAlignment);
		m_BufferSize = m_AlignmentSize * m_InstanceCount;
		m_Device.CreateBuffer(m_BufferSize, m_UsageFlags, m_MemoryPropertyFlags, m_Buffer, m_Memory);
	}

	Buffer::~Buffer()
	{
		UnMap();
		vkDestroyBuffer(m_Device.GetDevice(), m_Buffer, nullptr);
		vkFreeMemory(m_Device.GetDevice(), m_Memory, nullptr);
	}

	VkResult Buffer::Map(const VkDeviceSize pSize, const VkDeviceSize pOffset)
	{
		OWL_CORE_ASSERT(m_Buffer && m_Memory, "[VulkanBuffer] Called Map on buffer before create")
		return vkMapMemory(m_Device.GetDevice(), m_Memory, pOffset, pSize, 0, &m_Mapped);
	}

	void Buffer::UnMap()
	{
		if (m_Mapped)
		{
			vkUnmapMemory(m_Device.GetDevice(), m_Memory);
			m_Mapped = nullptr;
		}
	}

	void Buffer::WriteToBuffer(const void* pData, const VkDeviceSize pSize, const VkDeviceSize pOffset) const
	{
		OWL_CORE_ASSERT(m_Mapped, "Cannot copy to unmapped buffer")

		if (pSize == VK_WHOLE_SIZE)
			memcpy(m_Mapped, pData, m_BufferSize);
		else
		{
			auto memOffset = static_cast<char*>(m_Mapped);
			memOffset += pOffset;
			memcpy(memOffset, pData, pSize);
		}
	}

	VkResult Buffer::Flush(const VkDeviceSize pSize, const VkDeviceSize pOffset) const
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_Memory;
		mappedRange.offset = pOffset;
		mappedRange.size = pSize;
		return vkFlushMappedMemoryRanges(m_Device.GetDevice(), 1, &mappedRange);
	}

	VkDescriptorBufferInfo Buffer::DescriptorInfo(const VkDeviceSize pSize, const VkDeviceSize pOffset) const
	{
		return VkDescriptorBufferInfo{m_Buffer, pOffset, pSize};
	}

	VkResult Buffer::Invalidate(const VkDeviceSize pSize, const VkDeviceSize pOffset) const
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_Memory;
		mappedRange.offset = pOffset;
		mappedRange.size = pSize;
		return vkInvalidateMappedMemoryRanges(m_Device.GetDevice(), 1, &mappedRange);
	}

	void Buffer::WriteToIndex(const void* pData, const int pIndex) const
	{
		WriteToBuffer(pData, m_InstanceSize, pIndex * m_AlignmentSize);
	}

	VkResult Buffer::FlushIndex(const int pIndex) const
	{
		return Flush(m_AlignmentSize, pIndex * m_AlignmentSize);
	}

	VkDescriptorBufferInfo Buffer::DescriptorInfoForIndex(const int pIndex) const
	{
		return DescriptorInfo(m_AlignmentSize, pIndex * m_AlignmentSize);
	}

	VkResult Buffer::InvalidateIndex(const int pIndex) const
	{
		return Invalidate(m_AlignmentSize, pIndex * m_AlignmentSize);
	}

	VkDeviceSize Buffer::GetAlignment(const VkDeviceSize pInstanceSize, const VkDeviceSize pMinOffsetAlignment)
	{
		if (pMinOffsetAlignment > 0)
			return (pInstanceSize + pMinOffsetAlignment - 1) & ~(pMinOffsetAlignment - 1);
		return pInstanceSize;
	}
}
