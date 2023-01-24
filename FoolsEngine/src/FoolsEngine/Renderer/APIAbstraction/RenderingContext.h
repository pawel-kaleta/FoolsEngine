#pragma once

#include "FoolsEngine\Renderer\Renderer.h"

namespace fe
{
	class RenderingContext
	{
	public:
		virtual ~RenderingContext() = default;
		virtual void MakeCurrent() = 0;
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		inline static RenderCommands::APItype GetAPItype() { return RenderCommands::GetAPItype(); }
		static Scope<RenderingContext> Create(RenderCommands::APItype API, void* nativeWindow);
	};
}