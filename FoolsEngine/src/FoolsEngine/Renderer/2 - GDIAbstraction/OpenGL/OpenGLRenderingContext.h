#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\RenderingContext.h"

#include <GLFW/glfw3.h>

namespace fe
{
	class OpenGLRenderingContext : public RenderingContext
	{
	public:
		OpenGLRenderingContext(GLFWwindow* window);
		virtual ~OpenGLRenderingContext() override = default;

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual const GDIType GetGDItype() const override { return m_GDI; }
	private:
		GLFWwindow* m_Window;
		GDIType m_GDI;
	};
}
