#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\DeviceAPI.h"
#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"

namespace fe
{
	class RenderCommands
	{
	public:
		static Scope<DeviceAPI> CreateAPI(GDIType GDI);
		static void SetAPI(DeviceAPI* rendererAPI);
		static void InitAPI() { s_RendererAPI->Init(); }

		static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }

		static void Clear() { s_RendererAPI->Clear(); }

		static void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, uint32_t indexCount) { s_RendererAPI->DrawIndexed(vertexBuffer, indexCount); }
		static void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer) { s_RendererAPI->DrawIndexed(vertexBuffer); }

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_RendererAPI->SetViewport(x, y, width, height); }

		static void SetDepthTest(bool depthTets) { s_RendererAPI->SetDepthTest(depthTets); }

	private:
		static DeviceAPI* s_RendererAPI;
	};

}