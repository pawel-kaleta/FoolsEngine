#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\DeviceAPI.h"

namespace fe
{
	enum class GDIType;

	class RenderCommands
	{
	public:
		static Scope<DeviceAPI> CreateAPI(GDIType GDI);
		static void SetAPI(DeviceAPI* rendererAPI);
		static void InitAPI() { s_DeviceAPI->Init(); }

		static void SetClearColor(const glm::vec4& color) { s_DeviceAPI->SetClearColor(color); }

		static void Clear() { s_DeviceAPI->Clear(); }

		static void DrawIndexed(const VertexBuffer* vertexBuffer, uint32_t indexCount) { s_DeviceAPI->DrawIndexed(vertexBuffer, indexCount); }
		static void DrawIndexed(const VertexBuffer* vertexBuffer) { s_DeviceAPI->DrawIndexed(vertexBuffer); }

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_DeviceAPI->SetViewport(x, y, width, height); }

		static void SetDepthTest(bool depthTets) { s_DeviceAPI->SetDepthTest(depthTets); }

	private:
		static DeviceAPI* s_DeviceAPI;
	};

}