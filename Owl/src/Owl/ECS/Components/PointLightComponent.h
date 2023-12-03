#pragma once

namespace Owl
{
	struct PointLightComponent
	{
		float LightIntensity = 0.5f;
		glm::vec3 Color{1.f};
	};
}
