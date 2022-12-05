#pragma once

#include "FoolsEngine\Renderer\RenderingContext.h"

#include <GLFW/glfw3.h>

namespace fe
{
	class OpenGLContext : public RenderingContext
	{
	public:
		OpenGLContext(GLFWwindow* window);

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void ClearBuffer() override;
	private:
		GLFWwindow* m_Window;
	};
}
