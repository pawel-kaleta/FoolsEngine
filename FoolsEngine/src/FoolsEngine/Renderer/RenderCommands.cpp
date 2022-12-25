#include "FE_pch.h"
#include "RenderCommands.h"
//#include "FoolsEngine\Renderer\APIAbstraction\RenderingContext.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLRendererAPI.h"


namespace fe
{
	Scope<RendererAPI> RenderCommands::s_RendererAPI = Scope<RendererAPI>(nullptr);

	void RenderCommands::SetAPI(RendererAPI::NativeAPI nativeAPI)
	{
		switch (nativeAPI)
		{
		case RendererAPI::NativeAPI::none:
			FE_ASSERT(false, "RendererAPI type none!");
			return;
		case RendererAPI::NativeAPI::OpenGL:
			s_RendererAPI = CreateScope<OpenGLRendererAPI>();
			return;
		}

		FE_ASSERT(false, "Unknown RendererAPI type!");
	}
}