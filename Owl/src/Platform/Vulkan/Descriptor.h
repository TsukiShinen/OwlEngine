#pragma once
#include <memory>
#include <unordered_map>

#include "Device.h"

namespace Owl::Renderer::Vulkan
{
	class DescriptorSetLayout
	{
	public:
		class Builder
		{
		public:
			Builder(Device& pDevice) : m_Device(pDevice)
			{
			}

			Builder& AddBinding(uint32_t pBinding, VkDescriptorType pDescriptorType, VkShaderStageFlags pStageFlags,
			                    uint32_t pCount = 1);
			[[nodiscard]] std::unique_ptr<DescriptorSetLayout> Build() const;

		private:
			Device& m_Device;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;
		};

		DescriptorSetLayout(Device& pDevice,
		                    const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& pBindings);
		~DescriptorSetLayout();

		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

		[[nodiscard]] VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

	private:
		Device& m_Device;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;

		friend class DescriptorWriter;
	};

	class DescriptorPool
	{
	public:
		class Builder
		{
		public:
			Builder(Device& pDevice) : m_Device(pDevice)
			{
			}

			Builder& AddPoolSize(VkDescriptorType pDescriptorType, uint32_t pCount);
			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags pFlags);
			Builder& SetMaxSets(uint32_t pCount);

			[[nodiscard]] std::unique_ptr<DescriptorPool> Build() const;

		private:
			Device& m_Device;
			std::vector<VkDescriptorPoolSize> m_PoolSizes;
			uint32_t m_MaxSets = 1000;
			VkDescriptorPoolCreateFlags m_PoolFlags = 0;
		};

		DescriptorPool(Device& pDevice, uint32_t pMaxSets, VkDescriptorPoolCreateFlags pPoolFlags,
		               const std::vector<VkDescriptorPoolSize>& pPoolSizes);
		~DescriptorPool();

		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		bool AllocateDescriptor(VkDescriptorSetLayout pDescriptorSetLayout, VkDescriptorSet& pDescriptors) const;

		void FreeDescriptors(const std::vector<VkDescriptorSet>& pDescriptors) const;

		void ResetPool() const;

	private:
		Device& m_Device;
		VkDescriptorPool m_DescriptorPool;

		friend class DescriptorWriter;
	};

	class DescriptorWriter
	{
	public:
		DescriptorWriter(DescriptorSetLayout& pSetLayout, DescriptorPool& pPool);

		DescriptorWriter& WriteBuffer(uint32_t pBinding, const VkDescriptorBufferInfo* pBufferInfo);
		DescriptorWriter& WriteImage(uint32_t pBinding, const VkDescriptorImageInfo* pImageInfo);

		bool Build(VkDescriptorSet& pSet);
		void Overwrite(const VkDescriptorSet& pSet);

	private:
		DescriptorSetLayout& m_SetLayout;
		DescriptorPool& m_Pool;
		std::vector<VkWriteDescriptorSet> m_Writes;
	};
}
