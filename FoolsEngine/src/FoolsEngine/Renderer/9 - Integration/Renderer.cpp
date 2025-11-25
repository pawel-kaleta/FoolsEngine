#include "FE_pch.h"
#include "Renderer.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Framebuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\OpenGL\OpenGLShader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\3 - Representation\RenderMesh.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"
#include "FoolsEngine\Renderer\3 - Representation\Camera.h"
#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"
#include "FoolsEngine\Renderer\8 - Render\Renderer2D.h"
#include "FoolsEngine\Renderer\8 - Render\GeometryRenderer.h"

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
	GDIType Renderer::s_ActiveGDI = GDIType::None;
	std::unordered_map<GDIType::ValueType, Scope<DeviceAPI>> Renderer::s_DeviceAPIs;

	void Renderer::Startup()
	{
		FE_PROFILER_FUNC();
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

		auto& base_assets = Project::GetInstance()->BaseAssets;

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

		bool succes_1 = ShadingModel::DeserializeFromFile(BaseAssets.ShadingModels.Base3DOpaque.GetID(),	"../FoolsEngine/base_assets/shading_models/Base3DOpaque.fesm");
		bool succes_2 = ShadingModel::DeserializeFromFile(BaseAssets.ShadingModels.Base3DCutout.GetID(),	"../FoolsEngine/base_assets/shading_models/Base3DCutout.fesm");
		bool succes_3 = ShadingModel::DeserializeFromFile(BaseAssets.ShadingModels.Base3DBlend.GetID(),		"../FoolsEngine/base_assets/shading_models/Base3DBlend.fesm");

		FE_CORE_ASSERT(succes_1, "Failed to load Base3DOpaque shading model");
		FE_CORE_ASSERT(succes_2, "Failed to load Base3DCutout shading model");
		FE_CORE_ASSERT(succes_3, "Failed to load Base3DBlend shading model");

		BaseAssets.Materials.Default.Use().MakeMaterial(BaseAssets.ShadingModels.Base3DOpaque.Observe());

		UploadBaseAssetsToGPU(GetActiveGDItype());

		Renderer2D::s_Data.BaseShader = BaseAssets.Shaders.Base2D;
		Renderer2D::s_Data.Batch.Textures[0] = BaseAssets.Textures.FlatWhite.GetID();

		FE_LOG_CORE_INFO("Base Assets acquired");
	}

	void Renderer::UploadBaseAssetsToGPU(GDIType GDI)
	{
		FE_PROFILER_FUNC();

		BaseAssets.Textures.Default.Use().CreateGDITexture2D(GDI);
		BaseAssets.Textures.FlatWhite.Use().CreateGDITexture2D(GDI);

		ShaderLoader::CompileShader(GDI, BaseAssets.Shaders.Base2D.Use());
		ShaderLoader::CompileShader(GDI, BaseAssets.Shaders.Base3DOpaque.Use());
		ShaderLoader::CompileShader(GDI, BaseAssets.Shaders.Base3DCutout.Use());
		ShaderLoader::CompileShader(GDI, BaseAssets.Shaders.Base3DBlend.Use());
	}

	void Renderer::SetAPI(GDIType GDI)
	{
		FE_PROFILER_FUNC();
		
		FE_CORE_ASSERT(s_DeviceAPIs.find(GDI) != s_DeviceAPIs.end(), "API not created!");
		
		s_ActiveGDI = GDI;

		auto& deviceAPI = s_DeviceAPIs.at(GDI);

		RenderCommands::SetAPI(deviceAPI.get());

		Renderer2D::Init();
		GeometryRenderer::Init();
	}

	void Renderer::CreateAPI(GDIType GDI)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(s_DeviceAPIs.find(GDI) == s_DeviceAPIs.end(), "API already created!");

		s_DeviceAPIs[GDI] = RenderCommands::CreateAPI(GDI);
	}

	void Renderer::InitAPI(GDIType GDI)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(s_DeviceAPIs.find(GDI) != s_DeviceAPIs.end(), "API not created!");

		auto& deviceAPI = s_DeviceAPIs.at(GDI);

		deviceAPI->Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommands::SetViewport(0, 0, width, height);
	}

	void Renderer::RenderScene(const AssetObserver<Scene>& scene, const Camera& camera, const Transform& cameraTransform, Framebuffer& framebuffer)
	{
		framebuffer.Bind();

		int attachmentIndex = framebuffer.GetColorAttachmentIndex("EntityID");
		framebuffer.ClearAttachment(attachmentIndex, (uint32_t)NullEntityID);

		RenderScene(scene, camera, cameraTransform);

		framebuffer.Unbind();
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
		
		RenderCommands::Clear();
		RenderCommands::SetClearColor({ 0.1, 0.1, 0.1, 1 });

		switch (s_ActiveGDI.Value)
		{
		case GDIType::OpenGL:
			SceneData.VPMatrix = projection * glm::inverse(view);
			break;
		default:
			FE_CORE_ASSERT(false, "Unkown GDI!");
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

	//void Renderer::Draw(const Ref<VertexBuffer>& vertexBuffer, const AssetObserver<Material>& materialObserver, const glm::mat4& transform)
	//{
	//	Draw(vertexBuffer, materialObserver, transform, SceneData.VPMatrix);
	//}
	//
	//void Renderer::Draw(
	//	const Ref<VertexBuffer>& vertexBuffer,
	//	const AssetObserver<Material>& materialObserver,
	//	const glm::mat4& transform,
	//	const glm::mat4& VPMatrix)
	//{
	//	FE_PROFILER_FUNC();
	//
	//	auto& material_core = materialObserver.GetCoreComponent();
	//	AssetObserver<ShadingModel> sm_observer(material_core.ShadingModelID);
	//	auto& sm_core = sm_observer.GetCoreComponent();
	//	AssetUser<Shader> shaderUser(sm_core.ShaderID);
	//
	//	shaderUser.Bind(s_ActiveGDI);
	//
	//	shaderUser.UploadUniform(
	//		s_ActiveGDI,
	//		Uniform("u_ViewProjection", ShaderData::Type::Mat4),
	//		(void*)glm::value_ptr(VPMatrix)
	//	);
	//	shaderUser.UploadUniform(
	//		s_ActiveGDI,
	//		Uniform("u_Transform", ShaderData::Type::Mat4),
	//		(void*)glm::value_ptr(transform)
	//	);
	//
	//	for (const auto& uniform : sm_core.Uniforms)
	//	{
	//		auto dataPointer = materialObserver.GetUniformValuePtr(material_core, uniform);
	//		shaderUser.UploadUniform(s_ActiveGDI, uniform, dataPointer);
	//	}
	//
	//	{
	//		uint32_t rendererTextureSlot = 0;
	//		auto shaderTextureSlotsIt = sm_core.TextureSlots.begin();
	//
	//		for (const auto& textureID : material_core.TextureIDs)
	//		{
	//			shaderUser.BindTextureSlot(s_ActiveGDI, *shaderTextureSlotsIt++, rendererTextureSlot);
	//
	//			if (textureID)
	//			{
	//				AssetUser<Texture2D>(textureID).Bind(s_ActiveGDI, rendererTextureSlot++);
	//			}
	//			else
	//			{
	//				FE_LOG_CORE_WARN("Uninitialized texture!");
	//				BaseAssets.Textures.Default.Use().Bind(s_ActiveGDI, rendererTextureSlot++);
	//				continue;
	//			}
	//		}
	//	}
	//
	//	vertexBuffer->Bind();
	//
	//	RenderCommands::DrawIndexed(vertexBuffer.get());
	//}
} 