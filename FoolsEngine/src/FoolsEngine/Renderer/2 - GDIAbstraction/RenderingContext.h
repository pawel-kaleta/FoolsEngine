#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"

namespace fe
{
	class RenderingContext
	{
	public:
		virtual ~RenderingContext() = default;
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual const GDIType GetGDItype() const = 0;
		static Scope<RenderingContext> Create(GDIType GDI, void* nativeWindow);
	};
}