#pragma once
#include <memory>

#include "Owl/Core/Base.h"
#include "Owl/ECS/World.h"
#include "Owl/ECS/Systems/RenderSystem2D.h"
#include "Owl/Events/ApplicationEvent.h"
#include "Owl/Memory/Memory.h"
#include "Owl/Platform/Window.h"
#include "Platform/Vulkan/Api.h"
#include "Platform/Windows/WindowOld.h"

namespace Owl
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](const int pIndex) const
		{
			OWL_CORE_ASSERT(pIndex < Count);
			return Args[pIndex];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Owl Application";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	struct Version
	{
		int Major;
		int Minor;
		int Patch;
	};

	class Application
	{
	public:
		Application(ApplicationSpecification pSpecification);
		virtual ~Application();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagApplication); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagApplication); }

		void InitRenderer();

		void Close();

		virtual void InitializeEcs();
		virtual void InitializeEntities();
		virtual void PlaySystem();

		void Run();

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		static Application& Get() { return *s_Instance; }

	protected:
		virtual void OnEvent(Event& pEvent);
		
		bool OnWindowClose(WindowCloseEvent& pEvent);
		bool OnWindowResize(WindowResizeEvent& pEvent);
		
		/*static constexpr Version k_EngineVersion{0, 1, 0};
		static constexpr std::string k_EngineName = "Owl Engine";

		static constexpr Version k_ApplicationVersion{0, 1, 0};
		static constexpr std::string k_ApplicationName = "Test Owl Engine";
		int m_Width = 1280;
		int m_Height = 720;

		Renderer::Vulkan::Camera m_MainCamera{};
		//private:
		Scope<Window> m_Window;*/
		ApplicationSpecification m_Specification;

		bool m_IsRunning = true;
		bool m_IsMinimized = false;
		float m_LastFrameTime = 0.0f;

		/*Scope<Renderer::Vulkan::Api> m_VulkanApi;
		std::vector<Scope<Renderer::Vulkan::Buffer>> m_UboBuffers{Renderer::Vulkan::SwapChain::k_MaxFramesInFlight};
		std::vector<VkDescriptorSet> globalDescriptorSets{Renderer::Vulkan::SwapChain::k_MaxFramesInFlight};
		Scope<Renderer::Vulkan::DescriptorSetLayout> globalSetLayout;

		Ref<RenderSystem2D> m_RenderSystem2D;

	protected:
		Ecs::World m_World;*/

	private:
		Scope<Window> m_Window;
		
		static Application* s_Instance;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);
}
