#include "opch.h"
#include "World.h"

namespace Owl::Ecs
{
	World::~World()
	{
		OWL_PROFILE_FUNCTION();
	}

	void World::Initialize()
	{
		m_ComponentManager = std::make_unique<ComponentManager>();
		m_EntityManager = std::make_unique<EntityManager>();
		m_SystemManager = std::make_unique<SystemManager>();
	}

	Entity World::CreateEntity() const
	{
		return m_EntityManager->CreateEntity();
	}

	void World::DestroyEntity(const Entity pEntity) const
	{
		m_EntityManager->DestroyEntity(pEntity);
		m_ComponentManager->EntityDestroyed(pEntity);
		m_SystemManager->EntityDestroyed(pEntity);
	}
}
