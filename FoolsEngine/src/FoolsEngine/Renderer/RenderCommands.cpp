#include "FE_pch.h"
#include "RenderCommands.h"
//#include "FoolsEngine\Renderer\APIAbstraction\RenderingContext.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLRendererAPI.h"


namespace fe
{
	RendererAPI* RenderCommands::s_RendererAPI = nullptr;

	Scope<RendererAPI> RenderCommands::CreateAPI(GDIType GDI)
	{
		switch (GDI)
		{
		case GDIType::none:
			FE_ASSERT(false, "Cannot create RendererAPI for GDIType::none!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLRendererAPI>();
		default:
			FE_ASSERT(false, "Unknown GDIType!");
			return nullptr;
		}
	}

	void RenderCommands::SetAPI(RendererAPI* rendererAPI)
	{
		s_RendererAPI = rendererAPI;
	}
}