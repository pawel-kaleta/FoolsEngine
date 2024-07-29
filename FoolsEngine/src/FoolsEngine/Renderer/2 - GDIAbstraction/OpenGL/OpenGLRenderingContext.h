#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\RenderingContext.h"

#include <GLFW/glfw3.h>

namespace fe
{
	class OpenGLRenderingContext final : public RenderingContext
	{
	public:
		OpenGLRenderingContext() = default;
		OpenGLRenderingContext(void* nativeWindow);
		virtual ~OpenGLRenderingContext() override = default;

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* Window() const { return (GLFWwindow*)m_Window; };
	};
}
