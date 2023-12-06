#include "Renderer.h"

#include "Renderer/Vulkan/VulkanRendererApi.h"

namespace Owl
{
	RendererApi* Renderer::s_Instance = nullptr;

	void Renderer::Initialize(const std::string& pApplicationName)
	{
		s_Instance = new VulkanRendererApi(pApplicationName);
	}
}
