#pragma once

#include "FoolsEngine\Renderer\APIAbstraction\RendererAPI.h"
#include "GDIType.h"

namespace fe
{
	class RenderCommands
	{
	public:
		static Scope<RendererAPI> CreateAPI(GDIType GDI);
		static void SetAPI(RendererAPI* rendererAPI);
		inline static void InitAPI() { s_RendererAPI->Init(); }

		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }

		inline static void Clear() { s_RendererAPI->Clear(); }

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) { s_RendererAPI->DrawIndexed(vertexArray, indexCount); }
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray) { s_RendererAPI->DrawIndexed(vertexArray); }

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_RendererAPI->SetViewport(x, y, width, height); }

	private:
		static RendererAPI* s_RendererAPI;
	};

}