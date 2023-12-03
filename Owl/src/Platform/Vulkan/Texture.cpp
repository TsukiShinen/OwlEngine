#include "opch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Buffer.h"
#include "Descriptor.h"
#include "SwapChain.h"

namespace Owl::Renderer::Vulkan
{
	Texture::Texture(Device& pDevice, const char* pTexturePath)
		: m_Device(pDevice)
	{
		OWL_PROFILE_FUNCTION();
		CreateTextureImage(pTexturePath);
		CreateTextureImageView();
		CreateTextureSampler();
	}

	Texture::~Texture()
	{
		vkDestroySampler(m_Device.GetDevice(), m_TextureSampler, nullptr);
		vkDestroyImageView(m_Device.GetDevice(), m_TextureImageView, nullptr);

		vkDestroyImage(m_Device.GetDevice(), m_TextureImage, nullptr);
		vkFreeMemory(m_Device.GetDevice(), m_TextureImageMemory, nullptr);
	}

	void Texture::CreateTextureImage(const char* pTexturePath)
	{
		int textureWidth, textureHeight, textureChannels;
		stbi_uc* pixels = stbi_load(pTexturePath, &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = textureWidth * textureHeight * 4;

		if (!pixels)
			throw std::runtime_error("Failed to load texture image!");

		Buffer stagingBuffer{
			m_Device, imageSize, 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer(pixels);
		stbi_image_free(pixels);

		CreateImage(textureWidth, textureHeight, VK_FORMAT_R8G8B8A8_SRGB);
		TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
		                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		m_Device.CopyBufferToImage(stagingBuffer.GetBuffer(), m_TextureImage, static_cast<uint32_t>(textureWidth),
		                           static_cast<uint32_t>(textureHeight));
		TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void Texture::CreateImage(const uint32_t pWidth, const uint32_t pHeight, VkFormat pFormat)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = pWidth;
		imageInfo.extent.height = pHeight;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = pFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(m_Device.GetDevice(), &imageInfo, nullptr, &m_TextureImage) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_Device.GetDevice(), m_TextureImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = m_Device.FindMemoryType(memRequirements.memoryTypeBits,
		                                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(m_Device.GetDevice(), &allocInfo, nullptr, &m_TextureImageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(m_Device.GetDevice(), m_TextureImage, m_TextureImageMemory, 0);
	}

	void Texture::TransitionImageLayout(const VkImage pImage, VkFormat pFormat, const VkImageLayout pOldLayout,
	                                    const VkImageLayout pNewLayout) const
	{
		const VkCommandBuffer commandBuffer = m_Device.BeginSingleTimeGraphicsCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = pOldLayout;
		barrier.newLayout = pNewLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = pImage;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (pOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && pNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (pOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && pNewLayout ==
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		m_Device.EndSingleTimeGraphicsCommands(commandBuffer);
	}

	void Texture::CreateTextureImageView()
	{
		m_TextureImageView = m_Device.CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB);
	}

	void Texture::CreateTextureSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(m_Device.GetDevice(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture sampler!");
		}

		m_DescriptorPool = DescriptorPool::Builder(m_Device)
		                   .SetMaxSets(1)
		                   .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1)
		                   .Build();

		m_SpriteDescriptorSetLayout = DescriptorSetLayout::Builder(m_Device)
		                              .AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		                                          VK_SHADER_STAGE_FRAGMENT_BIT)
		                              .Build();

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_TextureImageView;
		imageInfo.sampler = m_TextureSampler;

		DescriptorWriter(*m_SpriteDescriptorSetLayout, *m_DescriptorPool)
			.WriteImage(0, &imageInfo)
			.Build(m_DescriptorSet);
	}
}
