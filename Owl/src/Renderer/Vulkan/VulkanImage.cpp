#include "VulkanImage.h"

#include "VulkanDevice.h"
#include "Owl/Debug/Assert.h"
#include "Owl/Debug/Instrumentor.h"

namespace Owl
{
	VulkanImage::VulkanImage(VulkanContext* pContext, const uint32_t pWidth, const uint32_t pHeight,
	                         const VkFormat pFormat, const VkImageTiling pTiling,
	                         const VkImageUsageFlags pUsage, VkMemoryPropertyFlags pMemoryFlags, const bool pCreateView,
	                         const VkImageAspectFlags pAspectFlags)
		: m_Context(pContext), m_Width(pWidth), m_Height(pHeight)
	{
		OWL_PROFILE_FUNCTION();
		VkImageCreateInfo imageCreateInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = m_Width;
		imageCreateInfo.extent.height = m_Height;
		imageCreateInfo.extent.depth = 1; // TODO: Support configurable depth.
		imageCreateInfo.mipLevels = 4; // TODO: Support mip mapping
		imageCreateInfo.arrayLayers = 1; // TODO: Support number of layers in the image.
		imageCreateInfo.format = pFormat;
		imageCreateInfo.tiling = pTiling;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = pUsage;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Configurable sample count.
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode.

		auto result = vkCreateImage(m_Context->Device->GetLogicalDevice(), &imageCreateInfo, m_Context->Allocator,
		                            &m_Handle);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanImage] Failed to create image handle!")

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(m_Context->Device->GetLogicalDevice(), m_Handle, &memoryRequirements);

		const int memoryType = m_Context->FindMemoryIndex(memoryRequirements.memoryTypeBits, pMemoryFlags);
		OWL_CORE_ASSERT(memoryType != -1, "Required memory type not found. Image not valid.")

		VkMemoryAllocateInfo memoryAllocateInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryType;

		result = vkAllocateMemory(m_Context->Device->GetLogicalDevice(), &memoryAllocateInfo, m_Context->Allocator,
		                          &m_Memory);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanImage] Failed to allocate memory!")

		result = vkBindImageMemory(m_Context->Device->GetLogicalDevice(), m_Handle, m_Memory, 0);
		// TODO: configurable memory offset.
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanImage] Failed to bind memory!")

		if (pCreateView)
		{
			CreateView(pFormat, pAspectFlags);
		}
	}

	VulkanImage::~VulkanImage()
	{
		OWL_PROFILE_FUNCTION();
		if (m_View)
			vkDestroyImageView(m_Context->Device->GetLogicalDevice(), m_View, m_Context->Allocator);
		if (m_Memory)
			vkFreeMemory(m_Context->Device->GetLogicalDevice(), m_Memory, m_Context->Allocator);
		if (m_Handle)
			vkDestroyImage(m_Context->Device->GetLogicalDevice(), m_Handle, m_Context->Allocator);
	}

	void VulkanImage::CreateView(const VkFormat pFormat, const VkImageAspectFlags pAspectFlags)
	{
		OWL_PROFILE_FUNCTION();
		VkImageViewCreateInfo viewCreateInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
		viewCreateInfo.image = m_Handle;
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: Make configurable.
		viewCreateInfo.format = pFormat;
		viewCreateInfo.subresourceRange.aspectMask = pAspectFlags;

		// TODO: Make configurable
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;

		const auto result = vkCreateImageView(m_Context->Device->GetLogicalDevice(), &viewCreateInfo,
		                                      m_Context->Allocator, &m_View);
		OWL_CORE_ASSERT(result == VK_SUCCESS, "[VulkanImage] Failed to create image view!")
	}
}
