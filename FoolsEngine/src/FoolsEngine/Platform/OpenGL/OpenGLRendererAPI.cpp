#include "FE_pch.h"
#include "OpenGLRendererAPI.h"

#include <glad\glad.h>

namespace fe
{

	void fe::OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		FE_PROFILER_FUNC();

		glClearColor(color.r, color.g, color.b, color.a);
	}

	void fe::OpenGLRendererAPI::Clear()
	{
		FE_PROFILER_FUNC();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void fe::OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		FE_PROFILER_FUNC();

		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

}