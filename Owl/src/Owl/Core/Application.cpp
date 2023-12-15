#include "opch.h"
#include "Application.h"

#include "Owl/ECS/Components/TransformComponent.h"
#include "Owl/Events/KeyEvent.h"
#include "Owl/Math/Vector2.h"
#include "Owl/Renderer/Renderer.h"

namespace Owl
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& pSpecification)
		: m_Specification(pSpecification), m_Window(nullptr)
	{
		OWL_PROFILE_FUNCTION();
		OWL_ASSERT(!s_Instance, "Application already exist!")
		s_Instance = this;
		
		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		// === Window ===
		m_Window = Window::Create(WindowProps(pSpecification.Name, 1280, 700));
		m_Window->SetEventCallback(OWL_BIND_EVENT_FN(Application::OnEvent));

		// === Renderer ===
		Renderer::Initialize(pSpecification.Name);
	}

	Application::~Application()
	{
		OWL_PROFILE_FUNCTION();
		Renderer::Shutdown();
		delete m_Window;
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

		while (m_IsRunning)
		{
			if (!m_IsMinimized)
			{
				OWL_PROFILE_SCOPE("RunLoop");

				if (Renderer::BeginFrame())
				{
					Renderer::EndFrame();
				}
			}

			m_Window->Update();
		}
	}

	Vector2 Application::GetFrameBufferSize() const
	{
		return Vector2(m_Window->GetWidth(), m_Window->GetHeight());
	}

	void Application::OnEvent(Event& pEvent)
	{
		OWL_PROFILE_FUNCTION();

		EventDispatcher dispatcher(pEvent);
		dispatcher.Dispatch<WindowCloseEvent>(OWL_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(OWL_BIND_EVENT_FN(Application::OnWindowResize));

		dispatcher.Dispatch<KeyPressedEvent>([](const KeyPressedEvent& pEvent1)
		{
			OWL_CORE_TRACE("Key pressed : %s", std::to_string(pEvent1.GetKeyCode()));
			return true;
		});
	}

	bool Application::OnWindowClose(WindowCloseEvent& pEvent)
	{
		m_IsRunning = false;
		return true;
	}

	bool Application::OnWindowResize(const WindowResizeEvent& pEvent)
	{
		OWL_PROFILE_FUNCTION();

		if (pEvent.GetWidth() == 0 || pEvent.GetHeight() == 0)
		{
			m_IsMinimized = true;
			return false;
		}

		m_IsMinimized = false;
		Renderer::Resize({static_cast<float>(pEvent.GetWidth()), static_cast<float>(pEvent.GetHeight())});

		return false;
	}
}
