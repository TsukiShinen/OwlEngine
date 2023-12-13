#pragma once

#include "Owl/Core/KeyCodes.h"
#include "Owl/Core/MouseCodes.h"
#include "Owl/Math/Vector2.h"

namespace Owl
{
	class Input
	{
	public:
		static bool IsKeyPressed(const KeyCode pKey) { return s_Keys[pKey]; }

		static bool IsMouseButtonPressed(const MouseCode pButton) { return s_MouseButtons[pButton]; }
		static Vector2 GetMousePosition() { return s_MousePosition; }
		static float GetMouseX() { return s_MousePosition.x; }
		static float GetMouseY() { return s_MousePosition.y; }

	private:
		static bool s_Keys[512];

		static bool s_MouseButtons[16];
		static Vector2 s_MousePosition;

		friend class WindowsWindow;
	};
}
