#pragma once

#include "GDIType.h"
#include "RenderCommands.h"
#include "OrthographicCamera.h"
#include "Material.h"

namespace fe
{
	class Renderer
	{
	public:
		inline const static GDIType GetActiveGDItype() { return s_ActiveGDI; }

		static void Init();
		static void SetAPI(GDIType GDI);
		static void CreateAPI(GDIType GDI);
		static void InitAPI(GDIType GDI);

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);
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

		struct GDIFixedSystems
		{
			GDIType GDI;
			Scope<RendererAPI> RendererAPI;
			Scope<ShaderLibrary> ShaderLib;
			Scope<TextureLibrary> TextureLib;
			Scope<MaterialLibrary> MaterialLib;
		};

		static Scope<SceneData> s_SceneData;
		static GDIType s_ActiveGDI;
		static std::unordered_map<GDIType, GDIFixedSystems> s_GDIFixedSystems;
	};
}