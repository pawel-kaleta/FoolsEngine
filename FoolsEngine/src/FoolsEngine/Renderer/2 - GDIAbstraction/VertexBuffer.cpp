#include "FE_pch.h"
#include "VertexBuffer.h"

#include "OpenGL\OpenGLVertexBuffer.h"

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

namespace fe
{
	Scope<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		FE_PROFILER_FUNC();
		switch (Renderer::GetActiveGDItype().Value)
		{
		case GDIType::None:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLVertexBuffer>(size);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}

	Scope<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		FE_PROFILER_FUNC();
		switch (Renderer::GetActiveGDItype().Value)
		{
		case GDIType::None:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLVertexBuffer>(vertices, size);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}
}