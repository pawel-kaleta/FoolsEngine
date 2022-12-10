#include "FE_pch.h"
#include "RenderCommands.h"
#include "FoolsEngine\Renderer\APIAbstraction\RenderingContext.h"

namespace fe
{
	std::unique_ptr<RendererAPI> RenderCommands::s_RendererAPI = std::unique_ptr<RendererAPI>(nullptr);
}