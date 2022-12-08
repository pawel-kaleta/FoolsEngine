#include "FE_pch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLVertexArray.h"

namespace fe
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case Renderer::RendererAPI::none:
			FE_CORE_ASSERT(false, "Renderer::RendererAPI::none currently not supported!");
			return nullptr;
		case Renderer::RendererAPI::OpenGL:
			return (VertexArray*) new OpenGLVertexArray();
		}

		FE_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}
}