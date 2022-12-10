#pragma once

#include "RenderCommands.h"

namespace fe
{

	class Renderer
	{
	public:
		inline static RendererAPI::NativeAPI GetNativeAPI() { return RendererAPI::GetNativeAPI(); }

		static void Init();
		static void SetAPI(RendererAPI* rendererAPI) { RenderCommands::SetAPI(rendererAPI); }

		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

	private:
		

	};
}