#pragma once

#include "FoolsEngine\Renderer\APIAbstraction\RenderingContext.h"

#include <GLFW/glfw3.h>

namespace fe
{
	class OpenGLRenderingContext : public RenderingContext
	{
	public:
		OpenGLRenderingContext(GLFWwindow* window);
		virtual ~OpenGLRenderingContext() override {};

		virtual void Init() override;
		virtual void MakeCurrent() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_Window;
	};
}
