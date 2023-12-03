#pragma once
#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <memory>
#include <glm/glm.hpp>

#include "Buffer.h"
#include "Texture.h"

namespace Owl::Renderer::Vulkan
{
	class Sprite
	{
	public:
		struct Vertex
		{
			glm::vec2 Position{};
			glm::vec3 Color{};
			glm::vec2 Normal{};
			glm::vec2 TexCoord{};

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

			bool operator==(const Vertex& pOther) const
			{
				return Position == pOther.Position &&
					Color == pOther.Color &&
					Normal == pOther.Normal &&
					TexCoord == pOther.TexCoord;
			}
		};

		Sprite(Device& pDevice, glm::vec2 pSize, glm::vec2 pOrigin);
		Sprite(Device& pDevice, glm::vec2 pSize, glm::vec2 pOrigin, const char* pTexturePath);
		~Sprite() = default;

		Sprite(const Sprite&) = delete;
		Sprite& operator=(const Sprite&) = delete;

		void Bind(VkCommandBuffer pCommandBuffer) const;
		void Draw(VkCommandBuffer pCommandBuffer) const;

		Scope<Texture>& GetTexture() { return m_Texture; }

	private:
		void CreateVertexBuffers(const std::vector<Vertex>& pVertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& pIndices);

		Device& m_Device;

		std::unique_ptr<Buffer> m_VertexBuffer;
		uint32_t m_VertexCount;

		bool m_HasIndexBuffer = false;
		std::unique_ptr<Buffer> m_IndexBuffer;
		uint32_t m_IndexCount;
		Scope<Texture> m_Texture;
	};
}
