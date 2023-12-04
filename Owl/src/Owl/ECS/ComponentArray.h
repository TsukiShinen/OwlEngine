#pragma once
#include <unordered_map>

#include "Ecs.h"

namespace Owl::Ecs
{
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(Entity pEntity) = 0;
	};

	template <typename T>
	class ComponentArray final : public IComponentArray
	{
	public:
		void InsertData(const Entity pEntity, T pComponent)
		{
			OWL_CORE_ASSERT(!m_EntityToIndexMap.contains(pEntity), "Component added to same entity more than once.")

			size_t newIndex = m_Size;
			m_EntityToIndexMap[pEntity] = newIndex;
			m_IndexToEntityMap[newIndex] = pEntity;
			m_ComponentArray[newIndex] = pComponent;
			++m_Size;
		}

		void RemoveData(const Entity pEntity)
		{
			OWL_CORE_ASSERT(m_EntityToIndexMap.contains(pEntity), "Removing non-existent component.")

			size_t indexOfRemovedEntity = m_EntityToIndexMap[pEntity];
			size_t indexOfLastElement = m_Size - 1;
			const Entity entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];

			m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];
			m_EntityToIndexMap[entityOfLastElement] = indexOfLastElement;
			m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

			m_EntityToIndexMap.erase(pEntity);
			m_IndexToEntityMap.erase(indexOfLastElement);

			--m_Size;
		}

		T& GetData(const Entity pEntity)
		{
			OWL_CORE_ASSERT(m_EntityToIndexMap.contains(pEntity), "Retrieving non-existent component.")

			return m_ComponentArray[m_EntityToIndexMap[pEntity]];
		}

		void EntityDestroyed(const Entity pEntity) override
		{
			if (m_EntityToIndexMap.contains(pEntity))
				RemoveData(pEntity);
		}

	private:
		std::array<T, MAX_ENTITIES> m_ComponentArray;
		std::unordered_map<Entity, size_t> m_EntityToIndexMap;
		std::unordered_map<size_t, Entity> m_IndexToEntityMap;
		size_t m_Size = 0;
	};
}
