#include "opch.h"
#include "InputManager.h"

#include "Owl/Event/SystemEventCode.h"

namespace Owl
{
	InputManager* InputManager::s_Instance = nullptr;

	void InputManager::Initialize()
	{
		OWL_PROFILE_FUNCTION();

		OWL_ASSERT(!s_Instance, "InputManager already exist!")
		s_Instance = new InputManager();
	}

	void InputManager::Shutdown()
	{
		delete s_Instance;
	}

	void InputManager::Update()
	{
		OWL_COPY_MEMORY(&m_KeyboardPrevious, &m_KeyboardCurrent, sizeof(Keyboard));
		OWL_COPY_MEMORY(&m_MousePrevious, &m_MouseCurrent, sizeof(Mouse));
	}

	bool InputManager::IsKeyDown(const Key pKey) const
	{
		return m_KeyboardCurrent.Keys[pKey] == true;
	}

	bool InputManager::IsKeyUp(const Key pKey) const
	{
		return m_KeyboardPrevious.Keys[pKey] == false;
	}

	bool InputManager::WasKeyDown(const Key pKey) const
	{
		return m_KeyboardCurrent.Keys[pKey] == true;
	}

	bool InputManager::WasKeyUp(const Key pKey) const
	{
		return m_KeyboardPrevious.Keys[pKey] == false;
	}

	void InputManager::ProcessKey(const Key pKey, const bool pIsPressed)
	{
		if (m_KeyboardCurrent.Keys[pKey] != pIsPressed)
		{
			m_KeyboardCurrent.Keys[pKey] = pIsPressed;

			EventContext context;
			context.Data.u16[0] = pKey;
			EventManager::Get()->Invoke(pIsPressed ? EventCodeKeyPressed : EventCodeKeyReleased, nullptr,
			                            context);
		}
	}

	bool InputManager::IsMouseButtonDown(const MouseButton pMouseButton) const
	{
		return m_MouseCurrent.Buttons[pMouseButton] == true;
	}

	bool InputManager::IsMouseButtonUp(const MouseButton pMouseButton) const
	{
		return m_MousePrevious.Buttons[pMouseButton] == false;
	}

	bool InputManager::WasMouseButtonDown(const MouseButton pMouseButton) const
	{
		return m_MouseCurrent.Buttons[pMouseButton] == true;
	}

	bool InputManager::WasMouseButtonUp(const MouseButton pMouseButton) const
	{
		return m_MousePrevious.Buttons[pMouseButton] == false;
	}

	glm::vec2 InputManager::GetMousePosition() const
	{
		return m_MouseCurrent.Position;
	}

	glm::vec2 InputManager::GetPreviousMousePosition() const
	{
		return m_MousePrevious.Position;
	}

	void InputManager::ProcessMouseButton(const MouseButton pMouseButton, const bool pIsPressed)
	{
		if (m_MouseCurrent.Buttons[pMouseButton] != pIsPressed)
		{
			m_MouseCurrent.Buttons[pMouseButton] = pIsPressed;

			EventContext context;
			context.Data.u16[0] = pMouseButton;
			EventManager::Get()->Invoke(pIsPressed ? EventCodeMouseButtonPressed : EventCodeMouseButtonReleased,
			                            nullptr, context);
		}
	}

	void InputManager::ProcessMouseMove(const glm::vec2 pPosition)
	{
		if (m_MouseCurrent.Position != pPosition)
		{
			m_MouseCurrent.Position = pPosition;

			EventContext context;
			context.Data.vec2[0] = pPosition;
			EventManager::Get()->Invoke(EventCodeMouseMoved, nullptr, context);
		}
	}

	void InputManager::ProcessMouseWheel(const int8_t pDelta)
	{
		EventContext context;
		context.Data.i8[0] = pDelta;
		EventManager::Get()->Invoke(EventCodeMouseWheel, nullptr, context);
	}
}
