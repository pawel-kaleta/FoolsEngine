#pragma once

#include "FoolsEngine/Foundation/Math/Transform.h"

#include "FoolsEngine/Assets/AssetHandle.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/2 - Representation/Mesh.h"
#include "FoolsEngine/Renderer/2 - Representation/Shader.h"
#include "FoolsEngine/Renderer/2 - Representation/Lights.h"
#include "FoolsEngine/Renderer/2 - Representation/Texture.h"
#include "FoolsEngine/Renderer/2 - Representation/Material.h"
#include "FoolsEngine/Renderer/2 - Representation/RenderMesh.h"
#include "FoolsEngine/Renderer/2 - Representation/ShadingModel.h"

namespace fe
{
	namespace Render::Representation { class Camera; }

	class Renderer
	{
	public:
		const static Render::GAPI::Platform GetActivePlatform() { return s_ActiveGAPI; }

		static void Startup();
		static void AcquireBaseAssets();
		static void UploadBaseAssetsToGPU(Render::GAPI::Platform GAPI);
		static void Shutdown();
		static void SetAPI(Render::GAPI::Platform GAPI);
		static void CreateAPI(Render::GAPI::Platform GAPI);
		static void InitAPI(Render::GAPI::Platform GAPI);

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void RenderScene(const AssetObserver& scene, const Render::Representation::Camera& camera, const Transform& cameraTransform);
		static void RenderScene(const AssetObserver& scene, const Render::Representation::Camera& camera, const Transform& cameraTransform, Render::GAPI::GID framebuffer);

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
			Render::Representation::DirectionalLight* MainLight;
			glm::vec3 AmbientLight;
			float AmbientLightIntensity;
			const Render::Representation::Camera* MainCamera;
			Transform CameraTransform;
			AssetID Scene;
		} SceneData;

	private:
		static Render::GAPI::Platform s_ActiveGAPI;
	};
}