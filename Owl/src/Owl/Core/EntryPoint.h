#pragma once
#include <iostream>

#include "Owl/Core/Base.h"
#include "Owl/Core/Application.h"
#include "Owl/Debug/Instrumentor.h"
#include "Owl/Memory/Memory.h"

#ifdef OWL_PLATFORM_WINDOWS

extern Owl::Application* CreateApplication(Owl::ApplicationCommandLineArgs pArgs);

void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagUnknown); }
void operator delete(void* pBlock, const size_t pSize) { return OWL_FREE(pBlock, pSize, Owl::MemoryTagUnknown); }

int main(int pArgc, char** pArgv)
{
	OWL_PROFILE_BEGIN_SESSION("Startup", "OwlProfile-Startup.json");
	Owl::Log::Initialize();
	const auto app = Owl::CreateApplication({pArgc, pArgv});
	OWL_PROFILE_END_SESSION();

	std::cout << "===== Memory after app initialization =====\n";
	std::cout << Owl::Memory::OwlGetMemoryUsageString();
	std::cout << "===========================================\n";

	OWL_PROFILE_BEGIN_SESSION("Runtime", "OwlProfile-Runtime.json");
	app->Run();
	OWL_PROFILE_END_SESSION();

	OWL_PROFILE_BEGIN_SESSION("Shutdown", "OwlProfile-Shutdown.json");
	delete app;
	Owl::Log::Shutdown();
	OWL_PROFILE_END_SESSION();

	std::cout << "===== Memory at the End of the app =====\n";
	std::cout << Owl::Memory::OwlGetMemoryUsageString();
	std::cout << "===========================================\n";
}

#endif
