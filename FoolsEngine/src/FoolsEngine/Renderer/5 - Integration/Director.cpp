#include "Director.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Scene/Scene.h"
#include "FoolsEngine/Scene/Component.h"
#include "FoolsEngine/Scene/GameplayWorld/Entity.h"
#include "FoolsEngine/Scene/Components/MeshComponents.h"
#include "FoolsEngine/Scene/Components/LightComponents.h"
#include "FoolsEngine/Scene/Components/RenderingComponents.h"

#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/Loaders/ShaderLoader.h"
#include "FoolsEngine/Assets/Loaders/TextureLoader.h"

#include "FoolsEngine/Renderer/1 - GAPI/Stream.h"
#include "FoolsEngine/Renderer/4 - Render/Renderer2D.h"
#include "FoolsEngine/Renderer/4 - Render/GeometryRenderer.h"

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

namespace fe::Render::Integration
{
	BaseAssetsStruct *	BaseAssets;
	SceneDataStruct	*	SceneData;

	TextureAllocator sTextureAllocator;
	GAPI::GID sTextureDownStream;
	GAPI::GID sSceneDataStream;

	namespace Director
	{
		void Startup()
		{
			FE_PROFILER_FUNC();
			Context::Log::ScopeCore("Renderer::Startup"_FE);

			sTextureAllocator.Init();
			sTextureAllocator.Create(100, 100 * 1024 * 1024 * 4, 200);
			sTextureDownStream = Render::GAPI::CreateDownStream();
			Render::GAPI::AllocateDownStream(sTextureDownStream, 5 * 4024 * 4024 * 4);

			sSceneDataStream = Render::GAPI::CreateDownStream();
			Render::GAPI::AllocateDownStream(sSceneDataStream, sizeof(SceneDataGPU) * 4);

			BaseAssets = Context::Alloc::Permanent.Allocate<BaseAssetsStruct>();
			SceneData = Context::Alloc::Permanent.Allocate<SceneDataStruct>();

			Renderer2D::Startup();
		}

		void Shutdown()
		{
			FE_PROFILER_FUNC();
			Renderer2D::Shutdown();
			GeometryRenderer::Shutdown();
		}

		template <typename tnAsset>
		inline void MakeHandle(const std::filesystem::path& path, AssetHandle<tnAsset>& hande, const UUID& uuid)
		{
			hande = AssetHandle<tnAsset>(AssetManager::AssetCreation::BaseAsset<tnAsset>(path, uuid), AssetLoadingPriority::None);
		}

		void AcquireBaseAssets()
		{
			FE_PROFILER_FUNC();

			auto& base_assets = Project::Get()->BaseAssets;

			{
				FE_PROFILER_SCOPE("Textures");

				MakeHandle("../FoolsEngine/base_assets/textures/Default_Texture.png", BaseAssets->Textures.Default, base_assets.Textures.Default);
				MakeHandle("../FoolsEngine/base_assets/textures/FlatWhite.png", BaseAssets->Textures.FlatWhite, base_assets.Textures.FlatWhite);
				MakeHandle("../FoolsEngine/base_assets/textures/FlatBlack.png", BaseAssets->Textures.FlatBlack, base_assets.Textures.FlatBlack);

				using namespace Render::GAPI::Descriptors::Texture::Sampling;

				BaseAssets->Textures.Default.Observe().Get<Texture2D::Core>().mViewSpecification = {
						.mWrapping = Wrapping::Repeat,
						.mFiltering = Filtering::Bilinear,
						.mMipmapping = Mipmapping::Liniear,
						.mASFiltering = ASFiltering::None
				};
				BaseAssets->Textures.FlatWhite.Observe().Get<Texture2D::Core>().mViewSpecification = {
						.mWrapping = Wrapping::Repeat,
						.mFiltering = Filtering::Nearest,
						.mMipmapping = Mipmapping::None,
						.mASFiltering = ASFiltering::None
				};
				BaseAssets->Textures.FlatBlack.Observe().Get<Texture2D::Core>().mViewSpecification = {
						.mWrapping = Wrapping::Repeat,
						.mFiltering = Filtering::Nearest,
						.mMipmapping = Mipmapping::None,
						.mASFiltering = ASFiltering::None
				};

				{
					FE_PROFILER_SCOPE("Loading");

					TextureLoader::LoadTexture("../FoolsEngine/base_assets/textures/Default_Texture.png", BaseAssets->Textures.Default.Use());
					TextureLoader::LoadTexture("../FoolsEngine/base_assets/textures/FlatWhite.png", BaseAssets->Textures.FlatWhite.Use());
					TextureLoader::LoadTexture("../FoolsEngine/base_assets/textures/FlatBlack.png", BaseAssets->Textures.FlatBlack.Use());
				}
			}

			{
				FE_PROFILER_SCOPE("Shaders");

				MakeHandle("../FoolsEngine/base_assets/Shaders/Base2DVertex.glsl", BaseAssets->Shaders.Base2DVertex, base_assets.Shaders.Base2DVertex);
				MakeHandle("../FoolsEngine/base_assets/Shaders/Base2DFragmentOpaque.glsl", BaseAssets->Shaders.Base2DFragmentOpaque, base_assets.Shaders.Base2DFragmentOpaque);
				MakeHandle("../FoolsEngine/base_assets/Shaders/Base2DFragmentCutout.glsl", BaseAssets->Shaders.Base2DFragmentCutout, base_assets.Shaders.Base2DFragmentCutout);
				MakeHandle("../FoolsEngine/base_assets/Shaders/Base2DFragmentBlend.glsl", BaseAssets->Shaders.Base2DFragmentBlend, base_assets.Shaders.Base2DFragmentBlend);
				MakeHandle("../FoolsEngine/base_assets/Shaders/Base3DVertex.glsl", BaseAssets->Shaders.Base3DVertex, base_assets.Shaders.Base3DVertex);
				MakeHandle("../FoolsEngine/base_assets/Shaders/Base3DFragmentBlend.glsl", BaseAssets->Shaders.Base3DFragmentBlend, base_assets.Shaders.Base3DFragmentBlend);
				MakeHandle("../FoolsEngine/base_assets/Shaders/Base3DFragmentCutout.glsl", BaseAssets->Shaders.Base3DFragmentCutout, base_assets.Shaders.Base3DFragmentCutout);
				MakeHandle("../FoolsEngine/base_assets/Shaders/Base3DFragmentOpaque.glsl", BaseAssets->Shaders.Base3DFragmentOpaque, base_assets.Shaders.Base3DFragmentOpaque);

				auto user_Base2DVertex = BaseAssets->Shaders.Base2DVertex.Use();
				auto user_Base2DFragmentOpaque = BaseAssets->Shaders.Base2DFragmentOpaque.Use();
				auto user_Base2DFragmentCutout = BaseAssets->Shaders.Base2DFragmentCutout.Use();
				auto user_Base2DFragmentBlend = BaseAssets->Shaders.Base2DFragmentBlend.Use();
				auto user_Base3DVertex = BaseAssets->Shaders.Base3DVertex.Use();
				auto user_Base3DFragmentBlend = BaseAssets->Shaders.Base3DFragmentBlend.Use();
				auto user_Base3DFragmentCutout = BaseAssets->Shaders.Base3DFragmentCutout.Use();
				auto user_Base3DFragmentOpaque = BaseAssets->Shaders.Base3DFragmentOpaque.Use();

				{
					using namespace Render::GAPI::Descriptors;

					user_Base2DVertex.Get<Shader::Core>().mType = ShaderType::Vertex;
					user_Base2DFragmentOpaque.Get<Shader::Core>().mType = ShaderType::Fragment;
					user_Base2DFragmentCutout.Get<Shader::Core>().mType = ShaderType::Fragment;
					user_Base2DFragmentBlend.Get<Shader::Core>().mType = ShaderType::Fragment;
					user_Base3DVertex.Get<Shader::Core>().mType = ShaderType::Vertex;
					user_Base3DFragmentBlend.Get<Shader::Core>().mType = ShaderType::Fragment;
					user_Base3DFragmentCutout.Get<Shader::Core>().mType = ShaderType::Fragment;
					user_Base3DFragmentOpaque.Get<Shader::Core>().mType = ShaderType::Fragment;
				}

				ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base2DVertex.glsl", user_Base2DVertex);
				ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base2DFragmentOpaque.glsl", user_Base2DFragmentOpaque);
				ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base2DFragmentCutout.glsl", user_Base2DFragmentCutout);
				ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base2DFragmentBlend.glsl", user_Base2DFragmentBlend);
				ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base3DVertex.glsl", user_Base3DVertex);
				ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base3DFragmentOpaque.glsl", user_Base3DFragmentBlend);
				ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base3DFragmentCutout.glsl", user_Base3DFragmentCutout);
				ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base3DFragmentBlend.glsl", user_Base3DFragmentOpaque);
			}

			{
				FE_PROFILER_SCOPE("Shading Models");

				MakeHandle("../FoolsEngine/base_assets/shading_models/Base2DFlatOpaque.fesm", BaseAssets->ShadingModels.Base2DFlatOpaque, base_assets.ShadingModels.Base2DFlatOpaque);
				MakeHandle("../FoolsEngine/base_assets/shading_models/Base2DFlatCutout.fesm", BaseAssets->ShadingModels.Base2DFlatCutout, base_assets.ShadingModels.Base2DFlatCutout);
				MakeHandle("../FoolsEngine/base_assets/shading_models/Base2DFlatBlend.fesm", BaseAssets->ShadingModels.Base2DFlatBlend, base_assets.ShadingModels.Base2DFlatBlend);
				MakeHandle("../FoolsEngine/base_assets/shading_models/Base3DOpaque.fesm", BaseAssets->ShadingModels.Base3DOpaque, base_assets.ShadingModels.Base3DOpaque);
				MakeHandle("../FoolsEngine/base_assets/shading_models/Base3DCutout.fesm", BaseAssets->ShadingModels.Base3DCutout, base_assets.ShadingModels.Base3DCutout);
				MakeHandle("../FoolsEngine/base_assets/shading_models/Base3DBlend.fesm", BaseAssets->ShadingModels.Base3DBlend, base_assets.ShadingModels.Base3DBlend);

				//bool succes_1 = BaseAssets.ShadingModels.Base2DFlat.Use().LoadBaseAssetMetadata("../FoolsEngine/base_assets/shading_models/Base2DFlat.fesm");
				//bool succes_2 = BaseAssets.ShadingModels.Base3DOpaque.Use().LoadBaseAssetMetadata("../FoolsEngine/base_assets/shading_models/Base3DOpaque.fesm");
				//bool succes_3 = BaseAssets.ShadingModels.Base3DCutout.Use().LoadBaseAssetMetadata("../FoolsEngine/base_assets/shading_models/Base3DCutout.fesm");
				//bool succes_4 = BaseAssets.ShadingModels.Base3DBlend.Use().LoadBaseAssetMetadata("../FoolsEngine/base_assets/shading_models/Base3DBlend.fesm");

				//FE_CORE_ASSERT(succes_1, "Failed to load Base2DFlat shading model");
				//FE_CORE_ASSERT(succes_2, "Failed to load Base3DOpaque shading model");
				//FE_CORE_ASSERT(succes_3, "Failed to load Base3DCutout shading model");
				//FE_CORE_ASSERT(succes_4, "Failed to load Base3DBlend shading model");
			}

			{
				FE_PROFILER_SCOPE("Materials");

				MakeHandle("Default2DFlatOpaque.femat", BaseAssets->Materials.Default2DFlatOpaque, base_assets.Materials.Default2DFlatOpaque);
				MakeHandle("Default2DFlatCutout.femat", BaseAssets->Materials.Default2DFlatCutout, base_assets.Materials.Default2DFlatCutout);
				MakeHandle("Default2DFlatBlend.femat", BaseAssets->Materials.Default2DFlatBlend, base_assets.Materials.Default2DFlatBlend);
				MakeHandle("DefaultOpaque.femat", BaseAssets->Materials.DefaultOpaque, base_assets.Materials.DefaultOpaque);
				MakeHandle("DefaultCutout.femat", BaseAssets->Materials.DefaultCutout, base_assets.Materials.DefaultCutout);
				MakeHandle("DefaultTranslucent.femat", BaseAssets->Materials.DefaultTranslucent, base_assets.Materials.DefaultTranslucent);

				//BaseAssets.Materials.Default2DBatchFlat.Use().MakeMaterial(BaseAssets.ShadingModels.Base2DFlat.Observe());
				//BaseAssets.Materials.DefaultOpaque.Use().MakeMaterial(BaseAssets.ShadingModels.Base3DOpaque.Observe());
				//BaseAssets.Materials.DefaultCutout.Use().MakeMaterial(BaseAssets.ShadingModels.Base3DCutout.Observe());
				//BaseAssets.Materials.DefaultTranslucent.Use().MakeMaterial(BaseAssets.ShadingModels.Base3DBlend.Observe());
			}

			Context::Log::Core.Error("AcquireBaseAssets() not implemented"_FE);

			Renderer2D::AcquireBaseAssets();

			Context::Log::Core.Info("Base Assets acquired"_FE);
		}

		void OnWindowResize(uint32_t width, uint32_t height)
		{
			Render::GAPI::SetViewportCmd(0, 0, width, height);
		}

		void RenderScene(const AssetObserver& scene, const Render::Representation::Camera& camera, const Transform& cameraTransform, Render::GAPI::GID framebuffer)
		{
			FE_PROFILER_FUNC();

			SceneData->Scene = scene.GetID();
			SceneData->MainCamera = &camera;
			SceneData->CameraTransform = cameraTransform;
			auto& gameplay_world = scene.Get<Scene::Core>().GameplayWorld;
			auto main_light_enity = gameplay_world->GetEntityWithPrimaryDirectionalLight();
			SceneData->MainLight = &main_light_enity.Get<CDirectionalLight>().DirectionalLight;

			BeginScene(camera, cameraTransform);

			auto scene_data_region = Render::GAPI::CreateRegion(sSceneDataStream, sizeof(SceneDataGPU), Render::GAPI::GetOffsetAlignmentFor<SceneDataGPU>());
			SceneDataGPU* scene_data_gpu = (SceneDataGPU*)Render::GAPI::GetRegionLocation(scene_data_region);

			*(glm::mat4*)&scene_data_gpu->VPMatrix = SceneData->VPMatrix;
			scene_data_gpu->MainLight = SceneData->MainLight->GetGPUStruct();
			*(glm::vec3*)&scene_data_gpu->CameraPosition = cameraTransform.Shift;
			*(glm::vec3*)&scene_data_gpu->AmbientLight = SceneData->AmbientLight;
			scene_data_gpu->AmbientLight.w = SceneData->AmbientLightIntensity;

			Render::GAPI::CommitRegion(scene_data_region);
			SceneData->sStreamOffsetIndex = Render::GAPI::GetRegionOffset(scene_data_region) / sizeof(SceneDataGPU);


			Renderer2D::RenderScene(scene, framebuffer);
			//GeometryRenderer::RenderScene(scene);	

			Render::GAPI::RetireRegionCmd(scene_data_region);

			EndScene();
		}

		void BeginScene(const glm::mat4& projection, const glm::mat4& view)
		{
			FE_PROFILER_FUNC();

			switch (Render::GAPI::Platform::FE_PARAMS_GAPI)
			{
			case Render::GAPI::Platform::OpenGL:
				SceneData->VPMatrix = projection * glm::inverse(view);
				//SceneData.VPMatrix = projection * view;
				break;
			default:
				FE_CORE_ASSERT(false, "Unkown GAPI!");
				return;
			}

			// TO DO: stats gathering
		}

		void EndScene()
		{
			FE_PROFILER_FUNC();
			// TO DO: stats gathering

			GLenum error = glGetError();

			if (error)
				Context::Log::Core.Info("{0}"_FE, error);
		}

	}
}