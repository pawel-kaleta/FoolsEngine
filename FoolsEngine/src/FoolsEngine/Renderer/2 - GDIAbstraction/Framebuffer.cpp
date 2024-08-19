#include "FE_pch.h"
#include "Framebuffer.h"

#include "OpenGL\OpenGLFramebuffer.h"

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

namespace fe
{
	Scope<Framebuffer> Framebuffer::Create(const FramebufferData::Specification& spec)
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