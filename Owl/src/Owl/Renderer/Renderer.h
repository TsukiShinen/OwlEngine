#pragma once
#include "RendererApi.h"
#include "glm/vec2.hpp"
#include "Owl/Core/Base.h"

namespace Owl
{
	class Renderer
	{
	public:
		static void Initialize(const std::string& pApplicationName);
		static void Shutdown() { delete s_Instance; }

		static void Resize(const glm::vec2 pSize) { s_Instance->Resize(pSize); }

		static bool BeginFrame() { return s_Instance->BeginFrame(); }
		static void EndFrame() { s_Instance->EndFrame(); }

	private:
		static RendererApi* s_Instance;
	};
}
