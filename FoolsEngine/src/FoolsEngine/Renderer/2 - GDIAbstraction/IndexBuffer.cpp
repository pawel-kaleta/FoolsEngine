#include "FE_pch.h"
#include "IndexBuffer.h"

#include "OpenGL\OpenGLIndexBuffer.h"

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

namespace fe
{
	Scope<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		FE_PROFILER_FUNC();
		switch (Renderer::GetActiveGDItype().Value)
		{
		case GDIType::None:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLIndexBuffer>(indices, count);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}
}