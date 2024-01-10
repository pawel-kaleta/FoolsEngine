#include "FE_pch.h"

#include "VertexArray.h"
#include "OpenGL\OpenGLVertexArray.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"

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