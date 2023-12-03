#pragma once
#include <list>
#include <map>
#include <typeindex>

#include "Ecs.h"

namespace Owl
{
	class HandlerFunctionBase
	{
	public:
		virtual ~HandlerFunctionBase() = default;

		void Execute(Event* pEvent)
		{
			Call(pEvent);
		}

	private:
		virtual void Call(Event* pEvent) = 0;
	};

	template <class T, class EventType>
	class MemberFunctionHandler final : public HandlerFunctionBase
	{
	public:
		using MemberFunction = void(T::*)(EventType*);

		MemberFunctionHandler(T* pInstance, const MemberFunction pMemberFunction)
			: m_Instance{pInstance}, m_MemberFunction{pMemberFunction}
		{
		}

		void Call(Event* pEvent) override
		{
			(m_Instance->*m_MemberFunction)(static_cast<EventType*>(pEvent));
		}

	private:
		T* m_Instance;
		MemberFunction m_MemberFunction;
	};


	using HandlerList = std::list<HandlerFunctionBase*>;

	class EventManager
	{
	public:
		template <typename EventType>
		void Publish(EventType* pEvent)
		{
			const HandlerList* handlers = m_Subscribers[typeid(EventType)];

			for (auto& handler : *handlers)
				handler->Execute(pEvent);
		}

		template <class T, class EventType>
		void Subscribe(T* pInstance, void (T::*pMemberFunction)(EventType*))
		{
			HandlerList* handlers = m_Subscribers[typeid(EventType)];

			if (handlers == nullptr)
			{
				handlers = new HandlerList();
				m_Subscribers[typeid(EventType)] = handlers;
			}

			handlers->push_back(new MemberFunctionHandler<T, EventType>(pInstance, pMemberFunction));
		}

	private:
		std::map<std::type_index, HandlerList*> m_Subscribers;
	};
}
