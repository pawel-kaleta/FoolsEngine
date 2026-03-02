#include "FE_pch.h"
#include "Renderer.h"

#include "FoolsEngine\Renderer\2 - Resource\Framebuffer.h"
#include "FoolsEngine\Renderer\3 - Command\PipelineState.h"
#include "FoolsEngine\Renderer\3 - Command\ResourceState.h"
#include "FoolsEngine\Renderer\4 - Representation\Texture.h"
#include "FoolsEngine\Renderer\4 - Representation\Shader.h"
#include "FoolsEngine\Renderer\4 - Representation\RenderMesh.h"
#include "FoolsEngine\Renderer\4 - Representation\Material.h"
#include "FoolsEngine\Renderer\4 - Representation\Mesh.h"
#include "FoolsEngine\Renderer\4 - Representation\Camera.h"
#include "FoolsEngine\Renderer\6 - Render\Renderer2D.h"
#include "FoolsEngine\Renderer\6 - Render\GeometryRenderer.h"

#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"
#include "FoolsEngine\Assets\Loaders\ShaderLoader.h"

#include "FoolsEngine\Scene\Scene.h"
#include "FoolsEngine\Scene\Component.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"

#include "FoolsEngine\Scene\Components\RenderingComponents.h"
#include "FoolsEngine\Scene\Components\MeshComponents.h"
#include "FoolsEngine\Scene\Components\LightComponents.h"

#include "FoolsEngine\Core\Project.h"

#include <glad\glad.h>

#include <glm\gtc\type_ptr.hpp>

namespace fe
{
	decltype(Renderer::SceneData) Renderer::SceneData;
	decltype(Renderer::BaseAssets) Renderer::BaseAssets;
	GAPIType Renderer::s_ActiveGAPI = GAPIType::None;
	//std::unordered_map<GAPIType::ValueType, Scope<DeviceAPI>> Renderer::s_DeviceAPIs;

	void Renderer::Startup()
	{
		FE_PROFILER_FUNC();

		Renderer2D::Startup();
	}

	void Renderer::Shutdown()
	{
		FE_PROFILER_FUNC();
		Renderer2D::Shutdown();
		GeometryRenderer::Shutdown();
	}

	template <typename tnAsset>
	void MakeHandle(const std::filesystem::path& path, AssetHandle<tnAsset>& hande, const UUID& uuid)
	{
		hande = AssetHandle<tnAsset>(AssetManager::AssetCreation::BaseAsset<tnAsset>(path, uuid), AssetLoadingPriority::None);
	}

	void Renderer::AcquireBaseAssets()
	{
		FE_PROFILER_FUNC();

		auto& base_assets = Project::Get()->BaseAssets;

		MakeHandle("../FoolsEngine/base_assets/textures/Default_Texture.png",	BaseAssets.Textures.Default,		base_assets.Textures.Default);
		MakeHandle("../FoolsEngine/base_assets/textures/FlatWhite.png",			BaseAssets.Textures.FlatWhite,		base_assets.Textures.FlatWhite);
		MakeHandle("../FoolsEngine/base_assets/textures/FlatBlack.png",			BaseAssets.Textures.FlatBlack,		base_assets.Textures.FlatBlack);

		MakeHandle("../FoolsEngine/base_assets/shaders/Base2DShader.glsl",	BaseAssets.Shaders.Base2D,			base_assets.Shaders.Base2D);
		MakeHandle("../FoolsEngine/base_assets/shaders/Base3DOpaque.glsl",	BaseAssets.Shaders.Base3DOpaque,	base_assets.Shaders.Base3DOpaque);
		MakeHandle("../FoolsEngine/base_assets/shaders/Base3DCutout.glsl",	BaseAssets.Shaders.Base3DCutout,	base_assets.Shaders.Base3DCutout);
		MakeHandle("../FoolsEngine/base_assets/shaders/Base3DBlend.glsl",	BaseAssets.Shaders.Base3DBlend,		base_assets.Shaders.Base3DBlend);

		MakeHandle("../FoolsEngine/base_assets/shading_models/Base3DOpaque.fesm",	BaseAssets.ShadingModels.Base3DOpaque,	base_assets.ShadingModels.Base3DOpaque);
		MakeHandle("../FoolsEngine/base_assets/shading_models/Base3DCutout.fesm",	BaseAssets.ShadingModels.Base3DCutout,	base_assets.ShadingModels.Base3DCutout);
		MakeHandle("../FoolsEngine/base_assets/shading_models/Base3DBlend.fesm",	BaseAssets.ShadingModels.Base3DBlend,	base_assets.ShadingModels.Base3DBlend);

		MakeHandle("Default.femat", BaseAssets.Materials.Default, base_assets.Materials.Default);
		// "Default.femat" is a dummy path for name

		TextureLoader::LoadTexture("../FoolsEngine/base_assets/textures/Default_Texture.png", BaseAssets.Textures.Default.Use());
		TextureLoader::LoadTexture("../FoolsEngine/base_assets/textures/FlatWhite.png"      , BaseAssets.Textures.FlatWhite.Use());
		TextureLoader::LoadTexture("../FoolsEngine/base_assets/textures/FlatBlack.png"      , BaseAssets.Textures.FlatBlack.Use());


		ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base2DShader.glsl", BaseAssets.Shaders.Base2D.Use());
		ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base3DOpaque.glsl", BaseAssets.Shaders.Base3DOpaque.Use());
		ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base3DCutout.glsl", BaseAssets.Shaders.Base3DCutout.Use());
		ShaderLoader::LoadShader("../FoolsEngine/base_assets/shaders/Base3DBlend.glsl",  BaseAssets.Shaders.Base3DBlend.Use());

		bool succes_1 = BaseAssets.ShadingModels.Base3DOpaque.Use().LoadBaseAssetMetadata("../FoolsEngine/base_assets/shading_models/Base3DOpaque.fesm");
		bool succes_2 = BaseAssets.ShadingModels.Base3DCutout.Use().LoadBaseAssetMetadata("../FoolsEngine/base_assets/shading_models/Base3DCutout.fesm");
		bool succes_3 = BaseAssets.ShadingModels.Base3DBlend.Use().LoadBaseAssetMetadata("../FoolsEngine/base_assets/shading_models/Base3DBlend.fesm");

		FE_CORE_ASSERT(succes_1, "Failed to load Base3DOpaque shading model");
		FE_CORE_ASSERT(succes_2, "Failed to load Base3DCutout shading model");
		FE_CORE_ASSERT(succes_3, "Failed to load Base3DBlend shading model");

		BaseAssets.Materials.Default.Use().MakeMaterial(BaseAssets.ShadingModels.Base3DOpaque.Observe());

		UploadBaseAssetsToGPU(GetActiveGAPIType());

		Renderer2D::Get().m_BaseShader = BaseAssets.Shaders.Base2D;
		Renderer2D::Get().m_Batch.Textures[0] = BaseAssets.Textures.FlatWhite.GetID();

		FE_LOG_CORE_INFO("Base Assets acquired");
	}

	void Renderer::UploadBaseAssetsToGPU(GAPIType GAPI)
	{
		FE_PROFILER_FUNC();

		BaseAssets.Textures.Default.Use().CreateResourceComponent<GAPIType::OpenGL>();
		BaseAssets.Textures.FlatWhite.Use().CreateResourceComponent<GAPIType::OpenGL>();

		ShaderLoader::CompileShader(GAPI, BaseAssets.Shaders.Base2D.Use());
		ShaderLoader::CompileShader(GAPI, BaseAssets.Shaders.Base3DOpaque.Use());
		ShaderLoader::CompileShader(GAPI, BaseAssets.Shaders.Base3DCutout.Use());
		ShaderLoader::CompileShader(GAPI, BaseAssets.Shaders.Base3DBlend.Use());
	}

	void Renderer::SetAPI(GAPIType GAPI)
	{
		FE_PROFILER_FUNC();
		
		//FE_CORE_ASSERT(s_DeviceAPIs.find(GAPI) != s_DeviceAPIs.end(), "API not created!");
		
		s_ActiveGAPI = GAPI;

		//auto& device_API = s_DeviceAPIs.at(GAPI);

		//RenderCommands::SetAPI(device_API.get());

		Renderer2D::Init();
		GeometryRenderer::Init();
	}

	void Renderer::CreateAPI(GAPIType GAPI)
	{
		FE_PROFILER_FUNC();

		//FE_CORE_ASSERT(s_DeviceAPIs.find(GAPI) == s_DeviceAPIs.end(), "API already created!");

		//s_DeviceAPIs[GAPI] = RenderCommands::CreateAPI(GAPI);
	}

	void Renderer::InitAPI(GAPIType GAPI)
	{
		FE_PROFILER_FUNC();

		//FE_CORE_ASSERT(s_DeviceAPIs.find(GAPI) != s_DeviceAPIs.end(), "API not created!");

		//auto& deviceAPI = s_DeviceAPIs.at(GAPI);

		//deviceAPI->Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		Command::PipelineState::SetViewport<GAPIType::OpenGL>(0, 0, width, height);
	}

	void Renderer::RenderScene(const AssetObserver<Scene>& scene, const Camera& camera, const Transform& cameraTransform, Resource::FramebufferBase& framebuffer)
	{
		Command::PipelineState::BindFramebuffer<GAPIType::OpenGL>(framebuffer);

		int attachment_index = framebuffer.GetColorAttachmentIndex("EntityID");
		Command::ResourceState::ClearAttachment<GAPIType::OpenGL>(framebuffer, attachment_index, (uint32_t)NullEntityID);

		RenderScene(scene, camera, cameraTransform);
	}

	void Renderer::RenderScene(const AssetObserver<Scene>& scene, const Camera& camera, const Transform& cameraTransform)
	{
		FE_PROFILER_FUNC();

		SceneData.Scene = scene.GetID();
		SceneData.MainCamera = &camera;
		SceneData.CameraTransform = cameraTransform;
		auto& gameplay_world = scene.GetCoreComponent().GameplayWorld;
		auto main_light_enity = gameplay_world->GetEntityWithPrimaryDirectionalLight();
		SceneData.MainLight = & main_light_enity.Get<CDirectionalLight>().DirectionalLight;

		BeginScene(camera, cameraTransform);

		Renderer2D::RenderScene(scene);
		GeometryRenderer::RenderScene(scene);	

		EndScene();
	}

	void Renderer::BeginScene(const glm::mat4& projection, const glm::mat4& view)
	{
		FE_PROFILER_FUNC();
		
		Command::ResourceState::Clear<GAPIType::OpenGL>();

		switch (s_ActiveGAPI.Value)
		{
		case GAPIType::OpenGL:
			SceneData.VPMatrix = projection * glm::inverse(view);
			break;
		default:
			FE_CORE_ASSERT(false, "Unkown GAPI!");
			return;
		}

		// TO DO: stats gathering
	}

	void Renderer::EndScene()
	{
		FE_PROFILER_FUNC();
		// TO DO: stats gathering

		GLenum error = glGetError();

		if (error)
			FE_LOG_CORE_INFO("{0}", error);
	}
} 