#pragma once
#include <memory>

#include "Owl/Core/Base.h"
#include "Owl/ECS/World.h"
#include "Owl/Events/ApplicationEvent.h"
#include "Owl/Platform/Window.h"

int main(int pArgc, char** pArgv);

namespace Owl
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](const int pIndex) const
		{
			OWL_CORE_ASSERT(pIndex < Count)
			return Args[pIndex];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Owl Application";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& pSpecification);
		virtual ~Application();

		virtual void OnEvent(Event& pEvent);

		void Close();

		[[nodiscard]] const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		[[nodiscard]] Window& GetWindow() const { return *m_Window; }
		[[nodiscard]] bool WindowExist() const { return m_Window != nullptr; }

		static Application* Get() { return s_Instance; }
		
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagApplication); }

		void operator delete(void* pBlock, const size_t pSize)
		{
			return OWL_FREE(pBlock, pSize, Owl::MemoryTagApplication);
		}
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& pEvent);
		bool OnWindowResize(const WindowResizeEvent& pEvent);

	private:
		ApplicationSpecification m_Specification;
		Scope<Window> m_Window;
		bool m_IsRunning = true;
		bool m_IsMinimized = false;
		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
		friend int ::main(int pArgc, char** pArgv);
	};

	Application* CreateApplication(ApplicationCommandLineArgs pArgs);
}
