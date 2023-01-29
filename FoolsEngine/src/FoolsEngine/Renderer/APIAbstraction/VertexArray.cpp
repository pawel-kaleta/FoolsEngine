#include "FE_pch.h"
#include "VertexArray.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLVertexArray.h"

namespace fe
{
	Scope<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetActiveGDItype())
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLVertexArray>();
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}
}