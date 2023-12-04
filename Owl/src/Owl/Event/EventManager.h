#pragma once
#include "Owl/Container/DArray.h"

namespace Owl
{
	using event_context = struct EventContext
	{
		// 128 bytes
		union
		{
			int64_t i64[2];
			uint64_t u64[2];
			glm::float64_t f64[2];

			int32_t i32[4];
			uint32_t u32[4];
			glm::float32_t f32[4];

			int16_t i16[8];
			uint16_t u16[8];

			glm::vec3 vec3[12];

			int8_t i8[16];
			uint8_t u8[16];
			double d[16];
			glm::vec2 vec2[16];

			char c[16];

			float f[32];
		} Data;
	};

	using Event = bool(*)(uint16_t pCode, void* pSender, void* pListener, EventContext pData);

	class EventManager
	{
	public:
		void Register(uint16_t pCode, void* pListener, Event pEvent);
		void UnRegister(uint16_t pCode, const void* pListener, Event pEvent) const;
		void Invoke(uint16_t pCode, void* pSender, const EventContext& pData) const;

		static EventManager* Get() { return s_Instance; }

	private:
		static void Initialize();
		static void Shutdown();

		struct RegisteredEvent
		{
			void* Listener;
			Event Callback;
		};

		struct EventCodeEntry
		{
			DArray<RegisteredEvent>* Events;
		};

		static const int k_MaxMessageCodes = 16384;

		EventCodeEntry m_Registered[k_MaxMessageCodes];

		static EventManager* s_Instance;
		friend class Application;
	};
}
