#pragma once

#include<glm\glm.hpp>

namespace fe
{
	class VertexBuffer;

	class DeviceAPI
	{
	public:
		
		virtual ~DeviceAPI() = default;

		virtual void Init() = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, uint32_t indexCount) = 0;
		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetDepthTest(bool depthTets) = 0;
	protected:
		
	};
}
