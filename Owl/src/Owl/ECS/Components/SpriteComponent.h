#pragma once
#include <glm/gtc/type_precision.inl>

#include "Platform/Vulkan/Sprite.h"

namespace Owl
{
	struct SpriteComponent
	{
		std::shared_ptr<Renderer::Vulkan::Sprite> Sprite{};
		glm::vec4 Color{1.f, 1.f, 1.f, 1.0f};
		int OrderLayer{0};
	};
}
