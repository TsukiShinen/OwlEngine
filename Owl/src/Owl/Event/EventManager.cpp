#include "opch.h"
#include "EventManager.h"

namespace Owl
{
	EventManager* EventManager::s_Instance;

	void EventManager::Initialize()
	{
		OWL_CORE_ASSERT(!s_Instance, "[EventManager] EventManager is already Initialize")

		s_Instance = new EventManager();
	}

	void EventManager::ShutDown()
	{
		for (const auto& element : s_Instance->m_Registered)
		{
			delete element.Events;
		}
	}

	void EventManager::Register(const uint16_t pCode, void* pListener, const Event pEvent)
	{
		OWL_CORE_ASSERT(s_Instance, "[EventManager] EventManager must be Initialize to Register an event")

		if (m_Registered[pCode].Events == nullptr)
			m_Registered[pCode].Events = new DArray<RegisteredEvent>();

		const uint64_t registeredSize = m_Registered[pCode].Events->size();
		for (uint64_t i = 0; i < registeredSize; i++)
		{
			if (m_Registered[pCode].Events->at(i).Listener == pListener)
			{
				OWL_CORE_WARN("[EventManager] Listener is already register to the event.");
				return;
			}
		}

		const RegisteredEvent myEvent{pListener, pEvent};
		m_Registered[pCode].Events->push_back(myEvent);
	}

	void EventManager::UnRegister(const uint16_t pCode, const void* pListener, const Event pEvent) const
	{
		OWL_CORE_ASSERT(s_Instance, "[EventManager] EventManager must be Initialize to UnRegister an event")

		if (m_Registered[pCode].Events == nullptr)
			OWL_CORE_WARN("[EventManager] Try to unregister a listener from an event empty.");

		const uint64_t registeredSize = m_Registered[pCode].Events->size();
		for (uint64_t i = 0; i < registeredSize; i++)
		{
			if (m_Registered[pCode].Events->at(i).Listener == pListener && m_Registered[pCode].Events->at(i).Callback ==
				pEvent)
			{
				m_Registered[pCode].Events->erase(m_Registered[pCode].Events->begin() + i);
				return;
			}
		}

		OWL_CORE_WARN("[EventManager] Listener was not register to the event.");
	}

	void EventManager::Invoke(const uint16_t pCode, void* pSender, const EventContext& pData) const
	{
		OWL_CORE_ASSERT(s_Instance, "[EventManager] EventManager must be Initialize to Invoke an event")

		if (m_Registered[pCode].Events == nullptr)
			OWL_CORE_WARN("[EventManager] Try to unregister a listener from an event empty.");

		const uint64_t registeredSize = m_Registered[pCode].Events->size();
		for (uint64_t i = 0; i < registeredSize; i++)
		{
			if (m_Registered[pCode].Events->at(i).Callback(pCode, pSender, m_Registered[pCode].Events->at(i).Listener,
			                                               pData))
			{
				return;
			}
		}
	}
}
