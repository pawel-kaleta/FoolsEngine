#include "FE_pch.h"
#include "OpenGLRendererAPI.h"

#include <glad\glad.h>

namespace fe
{
	void OpenGLRendererAPI::Init()
	{
		FE_LOG_CORE_INFO("OpenGLRendererAPI Initialization.");

		glEnable(GL_DEPTH_TEST);

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

		uint32_t indexCount = vertexArray->GetIndexBuffer()->GetCount();
		DrawIndexed(vertexArray, indexCount);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
	{
		FE_PROFILER_FUNC();

		if (indexCount == 0)
			return;
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetDepthTest(bool depthTets)
	{
		if (depthTets)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}


}