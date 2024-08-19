#include "FE_pch.h"
#include "RenderingContext.h"

#include "OpenGL\OpenGLRenderingContext.h"

namespace fe
{
	uint32_t RenderingContext::ContextsCount = 0;

	Scope<RenderingContext> RenderingContext::Create(GDIType GDI, void* nativeWindow)
	{
		switch (GDI)
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "Cannot create GDI-less rendering context!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLRenderingContext>(nativeWindow);
		default:
			FE_ASSERT(false, "Unknown GDI type!");
			return nullptr;
		}
	}
}