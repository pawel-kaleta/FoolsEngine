#include "FE_pch.h"
#include "VertexBuffer.h"

#include "OpenGL\OpenGLVertexBuffer.h"

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

namespace fe
{
	Scope<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		FE_PROFILER_FUNC();
		switch (Renderer::GetActiveGAPIType().Value)
		{
		case GAPIType::None:
			FE_CORE_ASSERT(false, "GAPIType::none currently not supported!");
			return nullptr;
		case GAPIType::OpenGL:
			return CreateScope<OpenGLVertexBuffer>(size);
		}

		FE_CORE_ASSERT(false, "Unknown GAPI");
		return nullptr;
	}

	Scope<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		FE_PROFILER_FUNC();
		switch (Renderer::GetActiveGAPIType().Value)
		{
		case GAPIType::None:
			FE_CORE_ASSERT(false, "GAPIType::none currently not supported!");
			return nullptr;
		case GAPIType::OpenGL:
			return CreateScope<OpenGLVertexBuffer>(vertices, size);
		}

		FE_CORE_ASSERT(false, "Unknown GAPI");
		return nullptr;
	}
}