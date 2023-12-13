#pragma once
#include "Owl/Math/Vector2.h"
#include "Owl/Memory/Memory.h"

namespace Owl
{
	class RendererApi
	{
	public:
		virtual ~RendererApi() = default;

		virtual void Resize(Vector2 pSize) = 0;

		virtual bool BeginFrame() = 0;
		virtual void EndFrame() = 0;
	};
}
