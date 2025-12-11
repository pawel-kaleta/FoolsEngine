#include "FE_pch.h"
#include "RenderingContext.h"

#include "OpenGL\OpenGLRenderingContext.h"

namespace fe
{
	uint32_t RenderingContext::ContextsCount = 0;

	Scope<RenderingContext> RenderingContext::Create(GAPIType GAPI, void* nativeWindow)
	{
		switch (GAPI.Value)
		{
		case GAPIType::None:
			FE_CORE_ASSERT(false, "Cannot create GAPI-less rendering context!");
			return nullptr;
		case GAPIType::OpenGL:
			return CreateScope<OpenGLRenderingContext>(nativeWindow);
		default:
			FE_ASSERT(false, "Unknown GAPI type!");
			return nullptr;
		}
	}
}