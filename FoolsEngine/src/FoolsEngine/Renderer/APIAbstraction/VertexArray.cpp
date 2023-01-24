#include "FE_pch.h"
#include "VertexArray.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLVertexArray.h"

namespace fe
{
	Scope<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPItype())
		{
		case RenderCommands::APItype::none:
			FE_CORE_ASSERT(false, "Renderer::RendererAPI::none currently not supported!");
			return nullptr;
		case RenderCommands::APItype::OpenGL:
			return CreateScope<OpenGLVertexArray>();
		}

		FE_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}
}