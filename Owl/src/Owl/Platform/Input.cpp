#include "opch.h"
#include "Input.h"

namespace Owl
{
	bool Input::s_Keys[512];

	bool Input::s_MouseButtons[16];
	glm::vec2 Input::s_MousePosition = glm::vec2();
}
