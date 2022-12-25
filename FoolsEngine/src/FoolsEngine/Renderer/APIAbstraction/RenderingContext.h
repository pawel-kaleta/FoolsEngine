#pragma once

#include "FoolsEngine\Renderer\Renderer.h"

namespace fe
{
	class RenderingContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		inline static RendererAPI::NativeAPI GetNativeAPI() { return RendererAPI::GetNativeAPI(); }

	};
}