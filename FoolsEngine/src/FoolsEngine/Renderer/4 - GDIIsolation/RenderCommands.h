#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\RendererAPI.h"
#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"

namespace fe
{
	class RenderCommands
	{
	public:
		static Scope<RendererAPI> CreateAPI(GDIType GDI);
		static void SetAPI(RendererAPI* rendererAPI);
		static void InitAPI() { s_RendererAPI->Init(); }

		static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }

		static void Clear() { s_RendererAPI->Clear(); }

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) { s_RendererAPI->DrawIndexed(vertexArray, indexCount); }
		static void DrawIndexed(const Ref<VertexArray>& vertexArray) { s_RendererAPI->DrawIndexed(vertexArray); }

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_RendererAPI->SetViewport(x, y, width, height); }

		static void SetDepthTest(bool depthTets) { s_RendererAPI->SetDepthTest(depthTets); }

	private:
		static RendererAPI* s_RendererAPI;
	};

}