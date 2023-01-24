#include "FE_pch.h"
#include "RenderingContext.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLRenderingContext.h"


namespace fe
{
	Scope<RenderingContext> RenderingContext::Create(RenderCommands::APItype API, void* nativeWindow)
	{
		switch (API)
		{
		case RenderCommands::APItype::none:
			FE_CORE_ASSERT(false, "Cannot create API-less rendering context!");
			return nullptr;
		case RenderCommands::APItype::OpenGL:
			return CreateScope<OpenGLRenderingContext>((GLFWwindow*)nativeWindow);
		default:
			FE_ASSERT(false, "Unknown RendererAPI type!");
			return nullptr;
		}
	}
}