#include <chrono>
#include <Owl.h>

#include "Components/MovementComponent.h"
#include "Owl/ECS/Components/TransformComponent.h"
#include "Systems/MovementControllerSystem.h"
#include "Owl/Core/EntryPoint.h"

class Sandbox final : public Owl::Application
{
public:
	Sandbox(const Owl::ApplicationSpecification& specification)
		: Application(specification)
	{
	}

	~Sandbox() override
	{
	}

	void InitializeEcs() override
	{
		/*
				Application::InitializeEcs();
		
				m_World.RegisterComponent<KeyboardInputComponent>();
				m_World.RegisterComponent<MovementComponent>();*/
	}

	void InitializeEntities() override
	{
		/*
				Application::InitializeEntities();
		
				m_CameraEntity = m_World.CreateEntity();
				m_World.AddComponent(m_CameraEntity, Owl::TransformComponent{
					                     .Position = {0.f, 0.f, -2.5f}
				                     });
				m_World.AddComponent(m_CameraEntity, KeyboardInputComponent{});
				m_World.AddComponent(m_CameraEntity, MovementComponent{
					                     .MoveSpeed = 3.0f,
					                     .RotationSpeed = 1.5f
				                     });
		
				auto sprite = m_World.CreateEntity();
				m_World.AddComponent(sprite, Owl::TransformComponent{});
				m_World.AddComponent(sprite, Owl::SpriteComponent{
					                     .Sprite = Owl::CreateRef<Owl::Renderer::Vulkan::Sprite>(m_VulkanApi->GetDevice(),
						                     glm::vec2{1, 1},
						                     glm::vec2{0.5f, 0.5f},
						                     "Assets/Textures/texture.jpg")
				                     });*/
	}

	void PlaySystem() override
	{
		Application::PlaySystem();
		/*
				const auto& cameraTransform = m_World.GetComponent<Owl::TransformComponent>(m_CameraEntity);
				m_MainCamera.SetViewYxz(cameraTransform.Position, cameraTransform.Rotation);*/
	}

private:
	//Owl::Ecs::Entity m_CameraEntity;
};

Owl::Application* Owl::CreateApplication(ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
