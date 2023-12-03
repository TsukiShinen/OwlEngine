#include "opch.h"
#include "Api.h"

namespace Owl::Renderer::Vulkan
{
	Api::Api(Scope<Window>& pWindow)
		: m_Window(pWindow), m_Device(m_Window), m_Renderer(m_Window, m_Device)
	{
		m_GlobalPool = DescriptorPool::Builder(m_Device)
		               .SetMaxSets(SwapChain::k_MaxFramesInFlight)
		               .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::k_MaxFramesInFlight)
		               .Build();
	}

	Api::~Api()
	{
		OWL_PROFILE_FUNCTION();
	}
}
