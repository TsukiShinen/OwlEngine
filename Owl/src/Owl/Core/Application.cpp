#include "opch.h"
#include "Application.h"

#include "Owl/ECS/Components/SpriteComponent.h"
#include "Owl/ECS/Components/TransformComponent.h"
#include "Owl/Events/KeyEvent.h"

namespace Owl
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification pSpecification)
		: m_Specification(pSpecification)
	{
		OWL_PROFILE_FUNCTION();

		OWL_ASSERT(!s_Instance, "Application already exist!")
		s_Instance = this;

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		m_Window = Window::Create(WindowProps(pSpecification.Name, 1280, 700));
		m_Window->SetEventCallback(OWL_BIND_EVENT_FN(Application::OnEvent));

		//m_Window = CreateScope<Window>(m_Width, m_Height, k_ApplicationName);

		//InitRenderer();
	}

	Application::~Application()
	{
		OWL_PROFILE_FUNCTION();
	}

	void Application::InitRenderer()
	{
		OWL_PROFILE_FUNCTION(); /*
		m_VulkanApi = CreateScope<Renderer::Vulkan::Api>(m_Window);

		for (int i = 0; i < m_UboBuffers.size(); i++)
		{
			m_UboBuffers[i] = std::make_unique<Renderer::Vulkan::Buffer>(
				m_VulkanApi->GetDevice(), sizeof(Renderer::Vulkan::GlobalUbo), 1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			m_UboBuffers[i]->Map();
		}

		globalSetLayout = Renderer::Vulkan::DescriptorSetLayout::Builder(m_VulkanApi->GetDevice())
		                  .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
		                  .Build();

		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = m_UboBuffers[i]->DescriptorInfo();
			Renderer::Vulkan::DescriptorWriter(*globalSetLayout, *m_VulkanApi->GetGlobalDescriptorPool())
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]);
		}

		m_MainCamera.SetViewTarget(glm::vec3{0.f}, glm::vec3{.0f, .0f, 2.5f});*/
	}

	void Application::Close()
	{
		m_IsRunning = false;
	}

	void Application::InitializeEcs()
	{
		OWL_PROFILE_FUNCTION();
		/*m_World.Initialize();

		m_World.RegisterComponent<TransformComponent>();
		m_World.RegisterComponent<SpriteComponent>();

		m_RenderSystem2D = m_World.RegisterSystem<RenderSystem2D>();

		Ecs::Signature renderSignature;
		renderSignature.set(m_World.GetComponentType<TransformComponent>());
		renderSignature.set(m_World.GetComponentType<SpriteComponent>());
		m_World.SetSystemSignature<RenderSystem2D>(renderSignature);

		m_RenderSystem2D->SetPipeLine(m_VulkanApi->GetDevice(), m_VulkanApi->GetRenderer().GetSwapChainRenderPass(),
		                              globalSetLayout->GetDescriptorSetLayout());*/
	}

	void Application::InitializeEntities()
	{
		OWL_PROFILE_FUNCTION();
	}

	void Application::PlaySystem()
	{
	}

	void Application::Run()
	{
		OWL_PROFILE_FUNCTION();
		OWL_CORE_INFO(Memory::OwlGetMemoryUsageString());

		while (m_IsRunning)
		{
			if (!m_IsMinimized)
			{
				OWL_PROFILE_SCOPE("RunLoop");
			}

			m_Window->Update();
		}
		/*
				auto currentTime = std::chrono::high_resolution_clock::now();
				while (!m_Window->ShouldClose())
				{
					OWL_PROFILE_SCOPE("RunLoop");
		
					glfwPollEvents();
		
					auto newTime = std::chrono::high_resolution_clock::now();
					m_LastFrameTime = std::chrono::duration<float>(newTime - currentTime).count();
					currentTime = newTime;
		
					const float aspect = m_VulkanApi->GetRenderer().GetAspectRatio();
					m_MainCamera.SetPerspectiveProjection(glm::radians(45.f), aspect, 0.1f, 100.f);
					PlaySystem();
		
					OWL_PROFILE_SCOPE("Render");
					if (const auto commandBuffer = m_VulkanApi->GetRenderer().BeginFrame())
					{
						const int frameIndex = m_VulkanApi->GetRenderer().GetFrameIndex();
						Renderer::Vulkan::FrameInfo frameInfo{
							frameIndex, m_LastFrameTime, commandBuffer, m_MainCamera, globalDescriptorSets[frameIndex]
						};
		
						Renderer::Vulkan::GlobalUbo ubo{};
						ubo.Projection = m_MainCamera.GetProjection();
						ubo.View = m_MainCamera.GetView();
						ubo.InverseView = m_MainCamera.GetInverseView();
						m_UboBuffers[frameIndex]->WriteToBuffer(&ubo);
						m_UboBuffers[frameIndex]->Flush();
		
						m_VulkanApi->GetRenderer().BeginSwapChainRenderPass(commandBuffer);
						m_RenderSystem2D->Render(frameInfo);
						m_VulkanApi->GetRenderer().EndSwapChainRenderPass(commandBuffer);
		
						m_VulkanApi->GetRenderer().EndFrame();
					}
				}
		
				vkDeviceWaitIdle(m_VulkanApi->GetDevice().GetDevice());*/
	}

	void Application::OnEvent(Event& pEvent)
	{
		OWL_PROFILE_FUNCTION();

		EventDispatcher dispatcher(pEvent);
		dispatcher.Dispatch<WindowCloseEvent>(OWL_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(OWL_BIND_EVENT_FN(Application::OnWindowResize));

		dispatcher.Dispatch<KeyPressedEvent>([](const KeyPressedEvent& pEvent1)
		{
			OWL_CORE_TRACE("Key pressed : " + std::to_string(pEvent1.GetKeyCode()));
			return true;
		});
	}

	bool Application::OnWindowClose(WindowCloseEvent& pEvent)
	{
		m_IsRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& pEvent)
	{
		OWL_PROFILE_FUNCTION();

		if (pEvent.GetWidth() == 0 || pEvent.GetHeight() == 0)
		{
			m_IsMinimized = true;
			return false;
		}

		m_IsMinimized = false;
		// Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}
