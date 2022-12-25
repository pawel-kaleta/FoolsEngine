#pragma once

#include "RenderCommands.h"
#include "OrtographicCamera.h"
#include "Material.h"

namespace fe
{

	class Renderer
	{
	public:
		inline static RendererAPI::NativeAPI GetNativeAPI() { return RendererAPI::GetNativeAPI(); }

		static void Init();
		static void SetAPI(RendererAPI::NativeAPI nativeAPI) { RenderCommands::SetAPI(nativeAPI); }

		static void BeginScene(OrtographicCamera& camera);
		static void EndScene();

		static void Submit(
			const std::shared_ptr<VertexArray>& vertexArray,
			const std::shared_ptr<MaterialInstance>& materialInstance,
			const glm::mat4& transform = glm::mat4(1.0f)
		);

	private:
		struct SceneData
		{
			glm::mat4 VPMatrix;
		};

		static Scope<SceneData> s_SceneData;
	};
}