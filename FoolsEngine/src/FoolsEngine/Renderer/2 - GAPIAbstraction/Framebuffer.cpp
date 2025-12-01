#include "FE_pch.h"
#include "Framebuffer.h"

#include "OpenGL\OpenGLFramebuffer.h"

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

namespace fe
{
	Scope<Framebuffer> Framebuffer::Create(const Description::Framebuffer::Specification& spec)
	{
		FE_PROFILER_FUNC();

		switch (Renderer::GetActiveGAPIType().Value)
		{
		case GAPIType::None:
			FE_CORE_ASSERT(false, "GAPIType::none currently not supported!");
			return nullptr;
		case GAPIType::OpenGL:
			return CreateScope<OpenGLFramebuffer>(spec);
		}

		FE_CORE_ASSERT(false, "Unknown GAPIType!");
		return nullptr;
	}
}