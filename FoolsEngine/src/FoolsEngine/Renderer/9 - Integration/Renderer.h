#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\DeviceAPI.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Framebuffer.h"
#include "FoolsEngine\Renderer\3 - Representation\Camera.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\MaterialInstance.h"

namespace fe
{
	class Renderer
	{
	public:
		const static GDIType GetActiveGDItype() { return s_ActiveGDI; }

		static void Startup();
		static void CreateBaseAssets();
		static void UploadBaseAssetsToGPU(GDIType GDI);
		static void Shutdown();
		static void SetAPI(GDIType GDI);
		static void CreateAPI(GDIType GDI);
		static void InitAPI(GDIType GDI);


		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Draw(
			const Ref<VertexBuffer>& vertexBuffer,
			AssetHandle<MaterialInstance> materialInstance,
			const glm::mat4& transform)
		{
			Draw(vertexBuffer, materialInstance, transform, s_SceneData->VPMatrix);
		}

		static void Draw(
			const Ref<VertexBuffer>& vertexBuffer,
			const AssetHandle<MaterialInstance> materialInstance,
			const glm::mat4& transform,
			const glm::mat4& VPMatrix
		);

	private:
		struct SceneData
		{
			glm::mat4 VPMatrix;
		};

		static Scope<SceneData> s_SceneData;
		static GDIType s_ActiveGDI;
		static std::unordered_map<GDIType, Scope<DeviceAPI>> s_DeviceAPIs;
	};
}