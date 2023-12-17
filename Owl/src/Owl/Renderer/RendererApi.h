#pragma once
#include "Owl/Math/Vector2.h"
#include "Owl/Memory/Memory.h"

namespace Owl
{
	class RendererApi
	{
	public:
		virtual ~RendererApi() = default;

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagRenderer); }

		void operator delete(void* pBlock, const size_t pSize)
		{
			return OWL_FREE(pBlock, pSize, Owl::MemoryTagRenderer);
		}

		virtual void Resize(Vector2 pSize) = 0;

		virtual bool BeginFrame() = 0;
		virtual void EndFrame() = 0;
	};
}
