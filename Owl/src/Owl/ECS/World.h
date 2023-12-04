#pragma once
#include <memory>

#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"

namespace Owl::Ecs
{
	class World
	{
	public:
		~World();
		void Initialize();

		Entity CreateEntity() const;
		void DestroyEntity(Entity pEntity) const;


		template <typename T>
		void RegisterComponent() const
		{
			m_ComponentManager->RegisterComponent<T>();
		}

		template <typename T>
		void AddComponent(const Entity pEntity, T pComponent)
		{
			m_ComponentManager->AddComponent<T>(pEntity, pComponent);

			auto signature = m_EntityManager->GetSignature(pEntity);
			signature.set(m_ComponentManager->GetComponentType<T>(), true);
			m_EntityManager->SetSignature(pEntity, signature);

			m_SystemManager->EntitySignatureChanged(pEntity, signature);
		}

		template <typename T>
		void RemoveComponent(const Entity pEntity) const
		{
			m_ComponentManager->RemoveComponent<T>(pEntity);

			auto signature = m_EntityManager->GetSignature(pEntity);
			signature.set(m_ComponentManager->GetComponentType<T>(), false);
			m_EntityManager->SetSignature(pEntity, signature);

			m_SystemManager->EntitySignatureChanged(pEntity, signature);
		}

		template <typename T>
		T& GetComponent(const Entity pEntity)
		{
			return m_ComponentManager->GetComponent<T>(pEntity);
		}

		template <typename T>
		[[nodiscard]] ComponentType GetComponentType() const
		{
			return m_ComponentManager->GetComponentType<T>();
		}


		template <typename T>
		std::shared_ptr<T> RegisterSystem()
		{
			return m_SystemManager->RegisterSystem<T>(this);
		}

		template <typename T>
		void SetSystemSignature(const Signature pSignature) const
		{
			m_SystemManager->SetSignature<T>(pSignature);
		}

	private:
		std::unique_ptr<ComponentManager> m_ComponentManager;
		std::unique_ptr<EntityManager> m_EntityManager;
		std::unique_ptr<SystemManager> m_SystemManager;
	};
}
