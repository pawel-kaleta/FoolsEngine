#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexArray.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\RendererAPI.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Framebuffer.h"
#include "FoolsEngine\Renderer\3 - Representation\Camera.h"
#include "FoolsEngine\Renderer\6 - Resources\MaterialLibrary.h"
#include "FoolsEngine\Renderer\6 - Resources\ShaderLibrary.h"
#include "FoolsEngine\Renderer\6 - Resources\TextureLibrary.h"

namespace fe
{
	class Renderer
	{
	public:
		const static GDIType GetActiveGDItype() { return s_ActiveGDI; }

		static void Init();
		static void Shutdown() {}
		static void SetAPI(GDIType GDI);
		static void CreateAPI(GDIType GDI);
		static void InitAPI(GDIType GDI);

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Draw(
			const Ref<VertexArray>& vertexArray,
			const Ref<MaterialInstance>& materialInstance,
			const glm::mat4& transform)
		{
			Draw(vertexArray, materialInstance, transform, s_SceneData->VPMatrix);
		}

		static void Draw(
			const Ref<VertexArray>& vertexArray,
			const Ref<MaterialInstance>& materialInstance,
			const glm::mat4& transform,
			const glm::mat4& VPMatrix
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