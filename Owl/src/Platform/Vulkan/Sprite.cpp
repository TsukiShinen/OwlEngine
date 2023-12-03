#include "opch.h"
#include "Sprite.h"

#include <glm/gtx/hash.hpp>

template <>
struct std::hash<Owl::Renderer::Vulkan::Sprite::Vertex>
{
	size_t operator()(const Owl::Renderer::Vulkan::Sprite::Vertex& pVertex) const noexcept
	{
		std::size_t seed = 0;
		Owl::HashCombine(seed, pVertex.Position, pVertex.Color, pVertex.Normal, pVertex.TexCoord);
		return seed;
	}
};

namespace Owl::Renderer::Vulkan
{
	std::vector<VkVertexInputBindingDescription> Sprite::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Sprite::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Position)});
		attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Color)});
		attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Normal)});
		attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, TexCoord)});

		return attributeDescriptions;
	}

	Sprite::Sprite(Device& pDevice, const glm::vec2 pSize, const glm::vec2 pOrigin = {0.f, 0.f})
		: m_Device(pDevice)
	{
		OWL_PROFILE_FUNCTION();
		const std::vector vertex = {
			Vertex{.Position = glm::vec2{-pOrigin}, .TexCoord = {0, 0}},
			Vertex{.Position = glm::vec2(-pOrigin + glm::vec2(pSize.x, 0.f)), .TexCoord = {1, 0}},
			Vertex{.Position = glm::vec2(-pOrigin + glm::vec2(0.f, pSize.y)), .TexCoord = {0, 1}},
			Vertex{.Position = glm::vec2(-pOrigin + pSize), .TexCoord = {1, 1}},
		};
		const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 1};

		CreateVertexBuffers(vertex);
		CreateIndexBuffers(indices);
	}

	Sprite::Sprite(Device& pDevice, const glm::vec2 pSize, const glm::vec2 pOrigin, const char* pTexturePath)
		: Sprite(pDevice, pSize, pOrigin)
	{
		OWL_PROFILE_FUNCTION();
		m_Texture = CreateScope<Texture>(m_Device, pTexturePath);
	}

	void Sprite::Bind(const VkCommandBuffer pCommandBuffer) const
	{
		const VkBuffer buffer[] = {m_VertexBuffer->GetBuffer()};
		constexpr VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, buffer, offsets);

		if (m_HasIndexBuffer)
			vkCmdBindIndexBuffer(pCommandBuffer, m_IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void Sprite::Draw(const VkCommandBuffer pCommandBuffer) const
	{
		if (m_HasIndexBuffer)
			vkCmdDrawIndexed(pCommandBuffer, m_IndexCount, 1, 0, 0, 0);
		else
			vkCmdDraw(pCommandBuffer, m_VertexCount, 1, 0, 0);
	}

	void Sprite::CreateVertexBuffers(const std::vector<Vertex>& pVertices)
	{
		m_VertexCount = static_cast<uint32_t>(pVertices.size());
		OWL_CORE_ASSERT(m_VertexCount >= 3, "[VulkanSprite] Vertex count must be at least 3!")

		const VkDeviceSize bufferSize = sizeof(pVertices[0]) * m_VertexCount;
		constexpr uint32_t vertexSize = sizeof(pVertices[0]);

		Buffer stagingBuffer{
			m_Device, vertexSize, m_VertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer(pVertices.data());

		m_VertexBuffer = std::make_unique<Buffer>(m_Device, vertexSize, m_VertexCount,
		                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_VertexBuffer->GetBuffer(), bufferSize);
	}

	void Sprite::CreateIndexBuffers(const std::vector<uint32_t>& pIndices)
	{
		m_IndexCount = static_cast<uint32_t>(pIndices.size());
		m_HasIndexBuffer = m_IndexCount > 0;
		if (!m_HasIndexBuffer) return;

		const VkDeviceSize bufferSize = sizeof(pIndices[0]) * m_IndexCount;
		constexpr uint32_t indexSize = sizeof(pIndices[0]);

		Buffer stagingBuffer{
			m_Device, indexSize, m_IndexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer(pIndices.data());

		m_IndexBuffer = std::make_unique<Buffer>(m_Device, indexSize, m_IndexCount,
		                                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_IndexBuffer->GetBuffer(), bufferSize);
	}
}
