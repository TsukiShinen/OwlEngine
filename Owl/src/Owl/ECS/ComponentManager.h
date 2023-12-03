#pragma once
#include <memory>
#include <ranges>
#include <unordered_map>

#include "ComponentArray.h"
#include "Ecs.h"

namespace Owl
{
	class ComponentManager
	{
	public:
		template <typename T>
		void RegisterComponent()
		{
			const char* typeName = typeid(T).name();

			OWL_CORE_ASSERT(!m_ComponentTypes.contains(typeName), "Registering component type more than once.");

			m_ComponentTypes.insert({typeName, m_NextComponentType});
			m_ComponentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
			++m_NextComponentType;
		}

		template <typename T>
		ComponentType GetComponentType()
		{
			const char* typeName = typeid(T).name();

			OWL_CORE_ASSERT(m_ComponentTypes.contains(typeName), "Component not registered before use.");

			return m_ComponentTypes[typeName];
		}

		template <typename T>
		void AddComponent(Entity pEntity, T pComponent)
		{
			GetComponentArray<T>()->InsertData(pEntity, pComponent);
		}

		template <typename T>
		void RemoveComponent(Entity pEntity)
		{
			GetComponentArray<T>()->RemoveData(pEntity);
		}

		template <typename T>
		T& GetComponent(Entity pEntity)
		{
			return GetComponentArray<T>()->GetData(pEntity);
		}

		void EntityDestroyed(const Entity pEntity) const
		{
			for (const auto& component : m_ComponentArrays | std::views::values)
				component->EntityDestroyed(pEntity);
		}

	private:
		std::unordered_map<const char*, ComponentType> m_ComponentTypes{};
		std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_ComponentArrays{};
		ComponentType m_NextComponentType{};

		template <typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray()
		{
			const char* typeName = typeid(T).name();

			OWL_CORE_ASSERT(m_ComponentTypes.contains(typeName), "Component not registered before use.");

			return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
		}
	};
}
