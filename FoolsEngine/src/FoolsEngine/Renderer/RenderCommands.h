#pragma once

#include "FoolsEngine\Renderer\APIAbstraction\RendererAPI.h"

namespace fe
{
	class RenderCommands
	{
	public:
		static void SetAPI(RendererAPI* rendererAPI) { s_RendererAPI.reset(rendererAPI); };
		inline static RendererAPI::NativeAPI GetNativeAPI() { return RendererAPI::GetNativeAPI(); }

		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }

		inline static void Clear() { s_RendererAPI->Clear(); }

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) { s_RendererAPI->DrawIndexed(vertexArray); }

	private:
		static std::unique_ptr<RendererAPI> s_RendererAPI;
	};

}