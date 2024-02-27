#include "FE_pch.h"
#include "OpenGLDeviceAPI.h"

#include "OpenGLVertexBuffer.h"

#include <glad\glad.h>

namespace fe
{
	void OpenGLDeviceAPI::Init()
	{
		FE_LOG_CORE_INFO("OpenGLDeviceAPI Initialization.");

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLDeviceAPI::SetClearColor(const glm::vec4& color)
	{
		FE_PROFILER_FUNC();

		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLDeviceAPI::Clear()
	{
		FE_PROFILER_FUNC();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLDeviceAPI::DrawIndexed(const VertexBuffer* vertexBuffer)
	{
		FE_PROFILER_FUNC();

		uint32_t indexCount = ((OpenGLVertexBuffer*)vertexBuffer)->GetIndexBuffer()->GetCount(); 
		DrawIndexed(vertexBuffer, indexCount);
	}

	void OpenGLDeviceAPI::DrawIndexed(const VertexBuffer* vertexBuffer, uint32_t indexCount)
	{
		FE_PROFILER_FUNC();

		if (indexCount == 0)
			return;
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLDeviceAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLDeviceAPI::SetDepthTest(bool depthTets)
	{
		if (depthTets)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
}