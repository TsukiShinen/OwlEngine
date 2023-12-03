#pragma once
#include "Owl/ECS/Ecs.h"
#include "Owl/ECS/World.h"
#include "Owl/ECS/Components/TransformComponent.h"

class RotateSystem : public Owl::System
{
public:
	void Rotate()
	{
		for (const auto entity : m_Entities)
		{
			auto& transform = m_World->GetComponent<Owl::TransformComponent>(entity);
			transform.Rotation.y += 0.0001f;
			transform.Rotation.x += 0.0001f;
			transform.Rotation.z += 0.0001f;
		}
	}
};
