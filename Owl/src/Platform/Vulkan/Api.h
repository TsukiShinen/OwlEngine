#pragma once
#include <memory>

#include "Descriptor.h"
#include "Device.h"
#include "Renderer.h"
#include "SwapChain.h"

namespace Owl::Renderer::Vulkan
{
	class Api
	{
	public:
		Api(Scope<Window>& pWindow);
		~Api();

		Api(const Api&) = delete;
		Api& operator=(const Api&) = delete;

		Device& GetDevice() { return m_Device; }
		Renderer& GetRenderer() { return m_Renderer; }

		std::unique_ptr<DescriptorPool>& GetGlobalDescriptorPool() { return m_GlobalPool; }

	private:
		Scope<Window>& m_Window;
		Device m_Device;
		Renderer m_Renderer;

		std::unique_ptr<DescriptorPool> m_GlobalPool{};
	};
}
