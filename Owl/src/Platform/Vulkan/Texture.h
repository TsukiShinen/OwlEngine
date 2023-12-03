#pragma once

#include "Descriptor.h"
#include "Device.h"

namespace Owl::Renderer::Vulkan
{
	class DescriptorWriter;

	class Texture
	{
	public:
		Texture(Device& pDevice, const char* pTexturePath);
		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		VkDescriptorSet& GetDescriptorSet() { return m_DescriptorSet; }

	private:
		void CreateTextureImage(const char* pTexturePath);
		void CreateImage(uint32_t pWidth, uint32_t pHeight, VkFormat pFormat);
		void TransitionImageLayout(VkImage pImage, VkFormat pFormat, VkImageLayout pOldLayout,
		                           VkImageLayout pNewLayout) const;
		void CreateTextureImageView();
		void CreateTextureSampler();

		Device& m_Device;

		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;

		VkImageView m_TextureImageView;
		VkSampler m_TextureSampler;

		VkDescriptorSet m_DescriptorSet;
		Scope<DescriptorSetLayout> m_SpriteDescriptorSetLayout;
		Scope<DescriptorPool> m_DescriptorPool;
	};
}
