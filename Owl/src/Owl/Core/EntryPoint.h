#pragma once
#include <iostream>

#include "Owl/Core/Base.h"
#include "Owl/Core/Application.h"
#include "Owl/Debug/Instrumentor.h"
#include "Owl/Memory/Memory.h"

#ifdef OWL_PLATFORM_WINDOWS

extern Owl::Application* CreateApplication(Owl::ApplicationCommandLineArgs args);

void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagUnknown); }
void operator delete(void* pBlock, const size_t pSize) { return OWL_FREE(pBlock, pSize, Owl::MemoryTagUnknown); }

static void PrintMemoryUsage()
{
	std::cout << "- Memory Usage : " << Owl::Memory::TotalAllocated << "B\n- Number of allocations : " << Owl::Memory::AllocationCount << "\n- " << Owl::Memory::OwlGetMemoryUsageString();
}

int main(int argc, char** argv)
{
	OWL_PROFILE_BEGIN_SESSION("Startup", "OwlProfile-Startup.json");
	auto app = Owl::CreateApplication({argc, argv});
	app->InitializeEcs();
	app->InitializeEntities();
	OWL_PROFILE_END_SESSION();
	
	std::cout << "===== Memory after app initialization =====\n";
	std::cout << Owl::Memory::OwlGetMemoryUsageString();
	std::cout << "===========================================\n";
	
	OWL_PROFILE_BEGIN_SESSION("Runtime", "OwlProfile-Runtime.json");
	app->Run();
	OWL_PROFILE_END_SESSION();

	OWL_PROFILE_BEGIN_SESSION("Shutdown", "OwlProfile-Shutdown.json");
	delete app;
	OWL_PROFILE_END_SESSION();
	
	std::cout << "===== Memory at the End of the app =====\n";
	std::cout << Owl::Memory::OwlGetMemoryUsageString();
}

#endif
