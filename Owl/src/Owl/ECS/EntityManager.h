#pragma once
#include <array>
#include <queue>

#include "Ecs.h"

namespace Owl
{
	class EntityManager
	{
	public:
		EntityManager();

		Entity CreateEntity();
		void DestroyEntity(Entity pEntity);
		void SetSignature(Entity pEntity, Signature pSignature);
		[[nodiscard]] Signature GetSignature(Entity pEntity) const;

	private:
		std::queue<Entity> m_AvailableEntities{};
		std::array<Signature, MAX_ENTITIES> m_Signatures{};
		uint32_t m_LivingEntityCount;
	};
}
