#pragma once

#include "FoolsEngine/Foundation/Math/Transform.h"

#include "FoolsEngine/Assets/AssetHandle.h"

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/5 - Representation/Mesh.h"
#include "FoolsEngine/Renderer/5 - Representation/Shader.h"
#include "FoolsEngine/Renderer/5 - Representation/Lights.h"
#include "FoolsEngine/Renderer/5 - Representation/Texture.h"
#include "FoolsEngine/Renderer/5 - Representation/Material.h"
#include "FoolsEngine/Renderer/5 - Representation/RenderMesh.h"
#include "FoolsEngine/Renderer/5 - Representation/ShadingModel.h"

namespace fe
{
	namespace Resource { struct FramebufferBase; }
	class Scene;
	class Camera;

	template <class tAssetType>
	class AssetHandle;

	template <class tAssetType>
	class AssetObserver;

	class Renderer
	{
	public:
		const static GAPIType GetActiveGAPIType() { return s_ActiveGAPI; }

		static void Startup();
		static void AcquireBaseAssets();
		static void UploadBaseAssetsToGPU(GAPIType GAPI);
		static void Shutdown();
		static void SetAPI(GAPIType GAPI);
		static void CreateAPI(GAPIType GAPI);
		static void InitAPI(GAPIType GAPI);

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void RenderScene(const AssetObserver<Scene>& scene, const Camera& camera, const Transform& cameraTransform);
		static void RenderScene(const AssetObserver<Scene>& scene, const Camera& camera, const Transform& cameraTransform, Resource::FramebufferBase& framebuffer);

		static void BeginScene(const glm::mat4& projection, const glm::mat4& view);
		static void EndScene();

		static struct BaseAssets // starting from C++20 msvc is unhappy about anonymous static properties :(
		{
			struct {
				AssetHandle<Texture2D> Default;
				AssetHandle<Texture2D> FlatWhite;
				AssetHandle<Texture2D> FlatBlack;
			} Textures;

			struct {
				AssetHandle<ShadingModel> Base2DBatchFlat;
				AssetHandle<ShadingModel> Base3DOpaque;
				AssetHandle<ShadingModel> Base3DCutout;
				AssetHandle<ShadingModel> Base3DBlend;
			} ShadingModels;

			struct {
				//AssetHandle<Material> Default2DBatchFlat; // do I need this material?
				AssetHandle<Material> DefaultOpaque;
				AssetHandle<Material> DefaultCutout;
				AssetHandle<Material> DefaultTranslucent;
			} Materials;
		} BaseAssets;

		static struct SceneData
		{
			glm::mat4 VPMatrix;
			DirectionalLight* MainLight;
			glm::vec3 AmbientLight;
			float AmbientLightIntensity;
			const Camera* MainCamera;
			Transform CameraTransform;
			AssetID Scene;
		} SceneData;

	private:
		static GAPIType s_ActiveGAPI;
	};
}