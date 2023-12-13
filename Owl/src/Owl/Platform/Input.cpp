#include "opch.h"
#include "Input.h"

namespace Owl
{
	bool Input::s_Keys[512];

	bool Input::s_MouseButtons[16];
	Vector2 Input::s_MousePosition = Vector2();
}
