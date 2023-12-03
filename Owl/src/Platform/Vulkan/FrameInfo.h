#pragma once
#include "Buffer.h"
#include "Camera.h"

namespace Owl::Renderer::Vulkan
{
#define MAX_LIGHTS 10

	struct PointLight
	{
		alignas(16) glm::vec3 Position{};
		alignas(16) glm::vec4 Color{};
	};

	struct GlobalUbo
	{
		alignas(16) glm::mat4 Projection{1.f};
		alignas(16) glm::mat4 View{1.f};
		alignas(16) glm::mat4 InverseView{1.f};
		alignas(16) glm::vec4 AmbientLightColor{1.f, 1.f, 1.f, 0.02f};
		alignas(16) PointLight PointLight[MAX_LIGHTS];
		alignas(16) int NumberLight;
	};

	struct FrameInfo
	{
		int FrameIndex;
		float FrameTime;
		VkCommandBuffer CommandBuffer;
		Camera Camera;
		VkDescriptorSet GlobalDescriptorSet;
	};
}
