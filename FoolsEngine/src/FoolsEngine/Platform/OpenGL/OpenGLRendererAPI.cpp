#include "FE_pch.h"
#include "OpenGLRendererAPI.h"

#include <glad\glad.h>

namespace fe
{
	void OpenGLRendererAPI::Init()
	{
		FE_LOG_CORE_INFO("OpenGLRendererAPI Initialization.");

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		FE_PROFILER_FUNC();

		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		FE_PROFILER_FUNC();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		FE_PROFILER_FUNC();

		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}


}