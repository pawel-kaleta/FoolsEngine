#pragma once

#include "RenderCommands.h"
#include "OrtographicCamera.h"

namespace fe
{

	class Renderer
	{
	public:
		inline static RendererAPI::NativeAPI GetNativeAPI() { return RendererAPI::GetNativeAPI(); }

		static void Init();
		static void SetAPI(RendererAPI* rendererAPI) { RenderCommands::SetAPI(rendererAPI); }

		static void BeginScene(OrtographicCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader);

	private:
		struct SceneData
		{
			glm::mat4 VPMatrix;
		};

		static SceneData* s_SceneData;
	};
}