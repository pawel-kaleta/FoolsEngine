#include "FE_pch.h"
#include "RenderCommands.h"
//#include "FoolsEngine\Renderer\APIAbstraction\RenderingContext.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLRendererAPI.h"


namespace fe
{
	RendererAPI* RenderCommands::s_RendererAPI = nullptr;
	RenderCommands::APItype RenderCommands::s_APItype = RenderCommands::APItype::none;

	Scope<RendererAPI> RenderCommands::CreateAPI(APItype API)
	{
		switch (API)
		{
		case RenderCommands::APItype::none:
			FE_ASSERT(false, "RendererAPI type none!");
			return nullptr;
		case RenderCommands::APItype::OpenGL:
			return CreateScope<OpenGLRendererAPI>();
		default:
			FE_ASSERT(false, "Unknown RendererAPI type!");
			return nullptr;
		}
	}

	void RenderCommands::SetAPI(RendererAPI* rendererAPI, APItype API)
	{
		s_RendererAPI = rendererAPI;
		s_APItype = API;
	}
}