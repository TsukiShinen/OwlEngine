#include "opch.h"
#include "SystemManager.h"

#include <ranges>

namespace Owl::Ecs
{
	void SystemManager::EntityDestroyed(const Entity pEntity)
	{
		for (const auto& system : m_Systems | std::views::values)
			system->GetEntities().erase(pEntity);
	}

	void SystemManager::EntitySignatureChanged(const Entity pEntity, const Signature pEntitySignature)
	{
		for (const auto& pair : m_Systems)
		{
			const auto& type = pair.first;
			const auto& system = pair.second;
			const auto& systemSignature = m_Signatures[type];

			if ((pEntitySignature & systemSignature) == systemSignature)
				system->GetEntities().insert(pEntity);
			else
				system->GetEntities().erase(pEntity);
		}
	}
}
