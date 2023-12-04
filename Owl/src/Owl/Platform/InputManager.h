#pragma once

#include "KeyCode.h"
#include "glm/vec2.hpp"

#define DEFINE_KEY(name, code) KEY_##name = code

namespace Owl
{
	class InputManager
	{
	public:
		[[nodiscard]] bool IsKeyDown(Key pKey) const;
		[[nodiscard]] bool IsKeyUp(Key pKey) const;
		[[nodiscard]] bool WasKeyDown(Key pKey) const;
		[[nodiscard]] bool WasKeyUp(Key pKey) const;

		void ProcessKey(Key pKey, bool pIsPressed);

		[[nodiscard]] bool IsMouseButtonDown(MouseButton pMouseButton) const;
		[[nodiscard]] bool IsMouseButtonUp(MouseButton pMouseButton) const;
		[[nodiscard]] bool WasMouseButtonDown(MouseButton pMouseButton) const;
		[[nodiscard]] bool WasMouseButtonUp(MouseButton pMouseButton) const;

		[[nodiscard]] glm::vec2 GetMousePosition() const;
		[[nodiscard]] glm::vec2 GetPreviousMousePosition() const;

		void ProcessMouseButton(MouseButton pMouseButton, bool pIsPressed);
		void ProcessMouseMove(glm::vec2 pPosition);
		void ProcessMouseWheel(int8_t pDelta);

		static InputManager* Get() { return s_Instance; }

	private:
		static void Initialize();
		static void Shutdown();

		void Update();

		struct Keyboard
		{
			bool Keys[MaxKeys];
		};

		struct Mouse
		{
			glm::vec2 Position;
			bool Buttons[MaxMouseButtons];
		};

		Keyboard m_KeyboardCurrent{};
		Keyboard m_KeyboardPrevious{};
		Mouse m_MouseCurrent{};
		Mouse m_MousePrevious{};

		static InputManager* s_Instance;
		friend class Application;
		friend class Windows;
	};
}
