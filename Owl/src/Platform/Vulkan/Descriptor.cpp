#include "opch.h"
#include "Descriptor.h"

#include <ranges>

namespace Owl::Renderer::Vulkan
{
	DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(uint32_t pBinding,
	                                                                       VkDescriptorType pDescriptorType,
	                                                                       VkShaderStageFlags pStageFlags,
	                                                                       uint32_t pCount)
	{
		OWL_CORE_ASSERT(m_Bindings.count(pBinding) == 0, "[DescriptorSetLayout::Builder] Binding alreading in use")
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = pBinding;
		layoutBinding.descriptorType = pDescriptorType;
		layoutBinding.descriptorCount = pCount;
		layoutBinding.stageFlags = pStageFlags;
		m_Bindings[pBinding] = layoutBinding;
		return *this;
	}

	std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const
	{
		return std::make_unique<DescriptorSetLayout>(m_Device, m_Bindings);
	}

	DescriptorSetLayout::DescriptorSetLayout(Device& pDevice,
	                                         const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>&
	                                         pBindings)
		: m_Device(pDevice), m_Bindings(pBindings)
	{
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
		for (auto val : m_Bindings | std::views::values)
		{
			setLayoutBindings.push_back(val);
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();


		if (vkCreateDescriptorSetLayout(m_Device.GetDevice(), &descriptorSetLayoutInfo, nullptr, &m_DescriptorSetLayout)
			!= VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor set layout!");
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_Device.GetDevice(), m_DescriptorSetLayout, nullptr);
	}

	DescriptorPool::Builder& DescriptorPool::Builder::AddPoolSize(VkDescriptorType pDescriptorType, uint32_t pCount)
	{
		m_PoolSizes.push_back({pDescriptorType, pCount});
		return *this;
	}

	DescriptorPool::Builder& DescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags pFlags)
	{
		m_PoolFlags = pFlags;
		return *this;
	}

	DescriptorPool::Builder& DescriptorPool::Builder::SetMaxSets(const uint32_t pCount)
	{
		m_MaxSets = pCount;
		return *this;
	}

	std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build() const
	{
		return std::make_unique<DescriptorPool>(m_Device, m_MaxSets, m_PoolFlags, m_PoolSizes);
	}

	DescriptorPool::DescriptorPool(Device& pDevice, const uint32_t pMaxSets,
	                               const VkDescriptorPoolCreateFlags pPoolFlags,
	                               const std::vector<VkDescriptorPoolSize>& pPoolSizes)
		: m_Device(pDevice)
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(pPoolSizes.size());
		descriptorPoolInfo.pPoolSizes = pPoolSizes.data();
		descriptorPoolInfo.maxSets = pMaxSets;
		descriptorPoolInfo.flags = pPoolFlags;

		if (vkCreateDescriptorPool(m_Device.GetDevice(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor pool!");
	}

	DescriptorPool::~DescriptorPool()
	{
		vkDestroyDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, nullptr);
	}

	bool DescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout pDescriptorSetLayout,
	                                        VkDescriptorSet& pDescriptors) const
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.pSetLayouts = &pDescriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

		if (vkAllocateDescriptorSets(m_Device.GetDevice(), &allocInfo, &pDescriptors) != VK_SUCCESS)
			return false;
		return true;
	}

	void DescriptorPool::FreeDescriptors(const std::vector<VkDescriptorSet>& pDescriptors) const
	{
		vkFreeDescriptorSets(m_Device.GetDevice(), m_DescriptorPool, pDescriptors.size(), pDescriptors.data());
	}

	void DescriptorPool::ResetPool() const
	{
		vkResetDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, 0);
	}

	DescriptorWriter::DescriptorWriter(DescriptorSetLayout& pSetLayout, DescriptorPool& pPool)
		: m_SetLayout(pSetLayout), m_Pool(pPool)
	{
	}

	DescriptorWriter& DescriptorWriter::WriteBuffer(const uint32_t pBinding, const VkDescriptorBufferInfo* pBufferInfo)
	{
		OWL_CORE_ASSERT(m_SetLayout.m_Bindings.count(pBinding) == 1,
		                "[DescriptorWriter] Layout does not contain specified binding!")

		const auto& bindingDescription = m_SetLayout.m_Bindings[pBinding];

		OWL_CORE_ASSERT(bindingDescription.descriptorCount == 1,
		                "[DescriptorWriter] Binding single descriptor info, but binding expects multiple!")

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = pBinding;
		write.pBufferInfo = pBufferInfo;
		write.descriptorCount = 1;

		m_Writes.push_back(write);
		return *this;
	}

	DescriptorWriter& DescriptorWriter::WriteImage(const uint32_t pBinding, const VkDescriptorImageInfo* pImageInfo)
	{
		OWL_CORE_ASSERT(m_SetLayout.m_Bindings.count(pBinding) == 1,
		                "[DescriptorWriter] Layout does not contain specified binding!")

		const auto& bindingDescription = m_SetLayout.m_Bindings[pBinding];

		OWL_CORE_ASSERT(bindingDescription.descriptorCount == 1,
		                "[DescriptorWriter] Binding single descriptor info, but binding expects multiple!")

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = pBinding;
		write.pImageInfo = pImageInfo;
		write.descriptorCount = 1;

		m_Writes.push_back(write);
		return *this;
	}

	bool DescriptorWriter::Build(VkDescriptorSet& pSet)
	{
		const bool success = m_Pool.AllocateDescriptor(m_SetLayout.GetDescriptorSetLayout(), pSet);
		if (!success)
			return false;
		Overwrite(pSet);
		return true;
	}

	void DescriptorWriter::Overwrite(const VkDescriptorSet& pSet)
	{
		for (auto& write : m_Writes)
			write.dstSet = pSet;
		vkUpdateDescriptorSets(m_Pool.m_Device.GetDevice(), m_Writes.size(), m_Writes.data(), 0, nullptr);
	}
}
