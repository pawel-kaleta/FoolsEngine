#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\DeviceAPI.h"

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\3 - Representation\RenderMesh.h"
#include "FoolsEngine\Renderer\3 - Representation\ShadingModel.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"
#include "FoolsEngine\Renderer\3 - Representation\Lights.h"

#include "FoolsEngine\Math\Transform.h"

#include "FoolsEngine\Assets\AssetHandle.h"

namespace fe
{
	class VertexBuffer;
	class Framebuffer;
	class Scene;
	class Camera;

	template <class tAssetType>
	class AssetHandle;

	template <class tAssetType>
	class AssetObserver;

	class Renderer
	{
	public:
		const static GDIType GetActiveGDItype() { return s_ActiveGDI; }

		static void Startup();
		static void AcquireBaseAssets();
		static void UploadBaseAssetsToGPU(GDIType GDI);
		static void Shutdown();
		static void SetAPI(GDIType GDI);
		static void CreateAPI(GDIType GDI);
		static void InitAPI(GDIType GDI);

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void RenderScene(const AssetObserver<Scene>& scene, const Camera& camera, const Transform& cameraTransform);
		static void RenderScene(const AssetObserver<Scene>& scene, const Camera& camera, const Transform& cameraTransform, Framebuffer& framebuffer);

		static void BeginScene(const glm::mat4& projection, const glm::mat4& view);
		static void EndScene();

		//static void Draw(
		//	const Ref<VertexBuffer>& vertexBuffer,
		//	const AssetObserver<Material>& materialObserver,
		//	const glm::mat4& transform
		//);
		//
		//static void Draw(
		//	const Ref<VertexBuffer>& vertexBuffer,
		//	const AssetObserver<Material>& materialObserver,
		//	const glm::mat4& transform,
		//	const glm::mat4& VPMatrix
		//);

		static struct BaseAssets // starting from C++20 msvc is unhappy about anonymous static properties :(
		{
			struct {
				AssetHandle<Texture2D> Default;
				AssetHandle<Texture2D> FlatWhite;
				AssetHandle<Texture2D> FlatBlack;
			} Textures;

			struct {
				AssetHandle<Shader> Base2D;
				AssetHandle<Shader> Base3DOpaque;
				AssetHandle<Shader> Base3DBlend;
			} Shaders;

			struct {
				AssetHandle<ShadingModel> Base3DOpaque;
				AssetHandle<ShadingModel> Base3DBlend;
			} ShadingModels;

			struct {
				AssetHandle<Material> Default;
			} Materials;
		} BaseAssets;

		static struct SceneData
		{
			glm::mat4 VPMatrix;
			DirectionalLight* MainLight;
			glm::vec3 AmbientLight;
			const Camera* MainCamera;
			Transform CameraTransform;
			AssetID Scene;
		} SceneData;

	private:
		static GDIType s_ActiveGDI;
		static std::unordered_map<GDIType::ValueType, Scope<DeviceAPI>> s_DeviceAPIs;
	};
}