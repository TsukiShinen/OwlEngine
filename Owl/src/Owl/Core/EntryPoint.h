#pragma once
#include "Owl/Core/Base.h"
#include "Owl/Core/Application.h"
#include "Owl/Debug/Instrumentor.h"
#include "Owl/Memory/Memory.h"

#ifdef OWL_PLATFORM_WINDOWS

extern Owl::Application* CreateApplication(Owl::ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	OWL_PROFILE_BEGIN_SESSION("Startup", "OwlProfile-Startup.json");
	auto app = Owl::CreateApplication({argc, argv});
	app->InitializeEcs();
	app->InitializeEntities();
	OWL_PROFILE_END_SESSION();

	OWL_PROFILE_BEGIN_SESSION("Runtime", "OwlProfile-Runtime.json");
	app->Run();
	OWL_PROFILE_END_SESSION();

	OWL_PROFILE_BEGIN_SESSION("Shutdown", "OwlProfile-Shutdown.json");
	delete app;
	OWL_PROFILE_END_SESSION();
}

#endif
