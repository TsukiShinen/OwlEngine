#include "opch.h"
#include "EntityManager.h"

namespace Owl::Ecs
{
	EntityManager::EntityManager()
		: m_LivingEntityCount(0)
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
			m_AvailableEntities.push(entity);
	}

	Entity EntityManager::CreateEntity()
	{
		OWL_CORE_ASSERT(m_LivingEntityCount < MAX_ENTITIES, "Too many entities in existence.")

		const Entity id = m_AvailableEntities.front();
		m_AvailableEntities.pop();
		++m_LivingEntityCount;

		return id;
	}

	void EntityManager::DestroyEntity(Entity pEntity)
	{
		OWL_CORE_ASSERT(pEntity < MAX_ENTITIES, "Requested to destroy an entity out of range.")

		m_Signatures[pEntity].reset();

		m_AvailableEntities.push(pEntity);
		--m_LivingEntityCount;
	}

	void EntityManager::SetSignature(Entity pEntity, Signature pSignature)
	{
		OWL_CORE_ASSERT(pEntity < MAX_ENTITIES, "Requested to set the signature of an entity out of range.")

		m_Signatures[pEntity] = pSignature;
	}

	Signature EntityManager::GetSignature(const Entity pEntity) const
	{
		OWL_CORE_ASSERT(pEntity < MAX_ENTITIES, "Requested to get the signature of an entity out of range.")

		return m_Signatures[pEntity];
	}
}
