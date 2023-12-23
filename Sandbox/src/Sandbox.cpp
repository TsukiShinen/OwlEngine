#include <chrono>
#include <Owl.h>

#include "Components/MovementComponent.h"
#include "Owl/ECS/Components/TransformComponent.h"
#include "Systems/MovementControllerSystem.h"
#include "Owl/Core/EntryPoint.h"

class Sandbox final : public Owl::Application
{
public:
	Sandbox(const Owl::ApplicationSpecification& pSpecification)
		: Application(pSpecification)
	{
	}

	~Sandbox() override
	{
	}
};

Owl::Application* Owl::CreateApplication(ApplicationCommandLineArgs pArgs)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.CommandLineArgs = pArgs;

	return new Sandbox(spec);
}
