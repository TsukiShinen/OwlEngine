#pragma once
#include "Owl/Math/Matrix4.h"
#include "Owl/Math/Vector3.h"


namespace Owl
{
	struct TransformComponent
	{
		Vector3 Position{};
		Vector3 Scale{1.f, 1.f, 1.f};
		Vector3 Rotation;
	};
}
