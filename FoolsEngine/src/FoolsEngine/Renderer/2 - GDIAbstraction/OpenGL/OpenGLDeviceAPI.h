#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\DeviceAPI.h"

namespace fe
{
	class OpenGLDeviceAPI : public DeviceAPI
	{
	public:
		OpenGLDeviceAPI() { FE_LOG_CORE_DEBUG("OpenGLDeviceAPI creation!"); };
		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const VertexBuffer* vertexBuffer, uint32_t indexCount = 0) override;
		virtual void DrawIndexed(const VertexBuffer* vertexBuffer) override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetDepthTest(bool depthTets) override;
	private:

	};
}

