#pragma once

#include "FoolsEngine\Renderer\APIAbstraction\RenderingContext.h"

#include <GLFW/glfw3.h>

namespace fe
{
	class OpenGLContext : public RenderingContext
	{
	public:
		OpenGLContext(GLFWwindow* window);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_Window;
	};
}
