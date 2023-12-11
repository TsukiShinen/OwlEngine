#pragma once
#include <memory>

#include "Owl/Core/Base.h"
#include "Owl/ECS/World.h"
#include "Owl/Events/ApplicationEvent.h"
#include "Owl/Memory/LinearAllocator.h"
#include "Owl/Memory/Memory.h"
#include "Owl/Platform/Window.h"

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
		Application(const ApplicationSpecification& pSpecification);
		virtual ~Application();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagApplication); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagApplication); }

		void Close();

		virtual void InitializeEcs();
		virtual void InitializeEntities();
		virtual void PlaySystem();

		void Run();
		[[nodiscard]] glm::vec2 GetFrameBufferSize() const;

		[[nodiscard]] const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		const Scope<Window>& GetWindow() { return m_Window; }
		void SetMinimized(const bool pValue) { m_IsMinimized = pValue; }

		static Application* Get() { return s_Instance; }

	protected:
		virtual void OnEvent(Event& pEvent);

		/*
	protected:
		Ecs::World m_World;*/

	private:
		bool OnWindowClose(WindowCloseEvent& pEvent);
		bool OnWindowResize(const WindowResizeEvent& pEvent);

		ApplicationSpecification m_Specification;

		bool m_IsRunning = true;
		bool m_IsMinimized = false;
		float m_LastFrameTime = 0.0f;
		
		Scope<Window> m_Window;

		LinearAllocator* m_SystemAllocator;

		static Application* s_Instance;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);
}
