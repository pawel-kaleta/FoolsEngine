#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\DeviceAPI.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Framebuffer.h"
#include "FoolsEngine\Renderer\3 - Representation\Camera.h"

#include "FoolsEngine\Math\Transform.h"
#include "FoolsEngine\Scene\Scene.h"

namespace fe
{
	class VertexBuffer;
	class Material;

	template <class Material>
	class AssetHandle;

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

		static void RenderScene(const AssetObserver<Scene>& scene, Framebuffer& framebuffer);
		static void RenderScene(const AssetObserver<Scene>& scene, const Camera& camera, const Transform& cameraTransform, Framebuffer& framebuffer);

		static void BeginScene(const glm::mat4& projection, const glm::mat4& view, Framebuffer& framebuffer);
		static void EndScene();

		static void Draw(
			const Ref<VertexBuffer>& vertexBuffer,
			AssetHandle<Material> material,
			const glm::mat4& transform
		);

		static void Draw(
			const Ref<VertexBuffer>& vertexBuffer,
			const AssetHandle<Material> material,
			const glm::mat4& transform,
			const glm::mat4& VPMatrix
		);

		static struct
		{
			struct {
				AssetHandle<Texture2D> Default;
				AssetHandle<Texture2D> FlatWhite;
			} Textures;

			struct {
				AssetHandle<Shader> Base2D;
				AssetHandle<Shader> Base3D;
			} Shaders;

			struct {
				AssetHandle<ShadingModel> Default;
			} ShadingModels;

			struct {
				AssetHandle<Material> Default;
			} Materials;
		} BaseAssets;
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