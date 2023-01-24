#pragma once

#include "RenderCommands.h"
#include "OrtographicCamera.h"
#include "Material.h"

namespace fe
{
	class Renderer
	{
	public:
		inline static RenderCommands::APItype GetAPItype() { return RenderCommands::GetAPItype(); }

		static void Init();
		static void SetAPI(RenderCommands::APItype API);
		static void CreateAPI(RenderCommands::APItype API);

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
		static std::unordered_map<RenderCommands::APItype, Scope<RendererAPI>> s_RenderingAPIs;
	};
}