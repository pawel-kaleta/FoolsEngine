#include "FE_pch.h"

#include "Buffers.h"
#include "OpenGL\OpenGLBuffers.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"

namespace fe
{
	Scope<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		FE_PROFILER_FUNC();
		switch (Renderer::GetActiveGDItype())
		{
		case GDIType::none:
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
		switch (Renderer::GetActiveGDItype())
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLVertexBuffer>(vertices, size);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}
	
	Scope<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		FE_PROFILER_FUNC();
		switch (Renderer::GetActiveGDItype())
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLIndexBuffer>(indices, count);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}

}