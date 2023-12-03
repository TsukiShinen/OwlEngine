#pragma once
#include "Device.h"

namespace Owl::Renderer::Vulkan
{
	class Buffer
	{
	public:
		Buffer(Device& pDevice, VkDeviceSize pInstanceSize, uint32_t pInstanceCount, VkBufferUsageFlags pUsageFlags,
		       VkMemoryPropertyFlags pMemoryPropertyFlags, VkDeviceSize pMinOffsetAlignment = 1);
		~Buffer();

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		VkResult Map(VkDeviceSize pSize = VK_WHOLE_SIZE, VkDeviceSize pOffset = 0);
		void UnMap();

		void WriteToBuffer(const void* pData, VkDeviceSize pSize = VK_WHOLE_SIZE, VkDeviceSize pOffset = 0) const;
		VkResult Flush(VkDeviceSize pSize = VK_WHOLE_SIZE, VkDeviceSize pOffset = 0) const;
		VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize pSize = VK_WHOLE_SIZE, VkDeviceSize pOffset = 0) const;
		VkResult Invalidate(VkDeviceSize pSize = VK_WHOLE_SIZE, VkDeviceSize pOffset = 0) const;

		void WriteToIndex(const void* pData, int pIndex) const;
		VkResult FlushIndex(int pIndex) const;
		VkDescriptorBufferInfo DescriptorInfoForIndex(int pIndex) const;
		VkResult InvalidateIndex(int pIndex) const;

		[[nodiscard]] VkBuffer GetBuffer() const { return m_Buffer; }
		[[nodiscard]] void* GetMappedMemory() const { return m_Mapped; }
		[[nodiscard]] uint32_t GetInstanceCount() const { return m_InstanceCount; }
		[[nodiscard]] VkDeviceSize GetInstanceSize() const { return m_InstanceSize; }
		[[nodiscard]] VkDeviceSize GetAlignmentSize() const { return m_AlignmentSize; }
		[[nodiscard]] VkBufferUsageFlags GetUsageFlags() const { return m_UsageFlags; }
		[[nodiscard]] VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return m_MemoryPropertyFlags; }
		[[nodiscard]] VkDeviceSize GetBufferSize() const { return m_BufferSize; }

	private:
		static VkDeviceSize GetAlignment(VkDeviceSize pInstanceSize, VkDeviceSize pMinOffsetAlignment);

		Device& m_Device;
		void* m_Mapped = nullptr;
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_Memory = VK_NULL_HANDLE;

		VkDeviceSize m_BufferSize;
		uint32_t m_InstanceCount;
		VkDeviceSize m_InstanceSize;
		VkDeviceSize m_AlignmentSize;
		VkBufferUsageFlags m_UsageFlags;
		VkMemoryPropertyFlags m_MemoryPropertyFlags;
	};
}
