#pragma once

#include<glm\glm.hpp>

namespace fe
{
	class VertexArray;

	class RendererAPI
	{
	public:
		
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetDepthTest(bool depthTets) = 0;
	protected:
		
	};
}