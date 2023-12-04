#pragma once
#include <memory>
#include <unordered_map>

#include "Ecs.h"

namespace Owl::Ecs
{
	class SystemManager
	{
	public:
		template <typename T>
		std::shared_ptr<T> RegisterSystem(World* pWorld)
		{
			const char* typeName = typeid(T).name();

			OWL_CORE_ASSERT(!m_Systems.contains(typeName), "Registering system more than once.")

			auto system = std::make_shared<T>(pWorld);
			m_Systems.insert({typeName, system});
			return system;
		}

		template <typename T>
		void SetSignature(Signature pSignature)
		{
			const char* typeName = typeid(T).name();

			OWL_CORE_ASSERT(m_Systems.contains(typeName), "System used before registered.")

			m_Signatures.insert({typeName, pSignature});
		}

		void EntityDestroyed(Entity pEntity);
		void EntitySignatureChanged(Entity pEntity, Signature pEntitySignature);

	private:
		std::unordered_map<const char*, Signature> m_Signatures{};
		std::unordered_map<const char*, std::shared_ptr<System>> m_Systems{};
	};
}
