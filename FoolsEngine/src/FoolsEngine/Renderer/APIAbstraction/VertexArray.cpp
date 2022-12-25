#include "FE_pch.h"
#include "VertexArray.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLVertexArray.h"

namespace fe
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetNativeAPI())
		{
		case RendererAPI::NativeAPI::none:
			FE_CORE_ASSERT(false, "Renderer::RendererAPI::none currently not supported!");
			return nullptr;
		case RendererAPI::NativeAPI::OpenGL:
			return static_cast<VertexArray*>(new OpenGLVertexArray());
		}

		FE_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}
}