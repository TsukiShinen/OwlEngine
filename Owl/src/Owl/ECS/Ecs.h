#pragma once
#include <bitset>
#include <set>


namespace Owl::Ecs
{
	class World;

	using Entity = std::uint32_t;
	constexpr Entity MAX_ENTITIES = 1000;

	using ComponentType = std::uint8_t;
	constexpr ComponentType MAX_COMPONENTS = 32;

	using Signature = std::bitset<MAX_COMPONENTS>;

	class System
	{
	public:
		System(World* pWorld)
			: m_World(pWorld)
		{
		}

		std::set<Entity>& GetEntities() { return m_Entities; }

	protected:
		std::set<Entity> m_Entities;
		World* m_World;
	};
}
