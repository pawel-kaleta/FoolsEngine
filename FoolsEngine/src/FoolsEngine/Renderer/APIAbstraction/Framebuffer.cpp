#include "FE_pch.h"
#include "Framebuffer.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLFramebuffer.h"

namespace fe
{
	Scope<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		FE_PROFILER_FUNC();

		switch (Renderer::GetActiveGDItype())
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLFramebuffer>(spec);
		}

		FE_CORE_ASSERT(false, "Unknown GDIType!");
		return nullptr;
	}
}