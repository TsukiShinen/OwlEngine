#pragma once
#include "glm/vec2.hpp"
#include "Owl/Memory/Memory.h"

namespace Owl
{
	class RendererApi
	{
	public:
		virtual ~RendererApi() = default;

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagRenderer); }

		virtual void Resize(glm::vec2 pSize) = 0;

		virtual bool BeginFrame() = 0;
		virtual void EndFrame() = 0;
	};
}
