#include "FE_pch.h"
#include "Renderer.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\OpenGL\OpenGLShader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"
#include "FoolsEngine\Renderer\3 - Representation\Camera.h"
#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"
#include "FoolsEngine\Renderer\8 - Render\Renderer2D.h"

#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"
#include "FoolsEngine\Assets\Loaders\ShaderLoader.h"

#include "FoolsEngine\Scene\Scene.h"
#include "FoolsEngine\Scene\Component.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"

#include "FoolsEngine\Scene\Components\RenderingComponents.h"
#include "FoolsEngine\Scene\Components\MeshComponents.h"

#include "FoolsEngine\Core\Project.h"

#include <glad\glad.h>

#include <glm\gtc\type_ptr.hpp>

namespace fe
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
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
	}

	template <typename tnAsset>
	void MakeHandle(const std::filesystem::path& path, AssetHandle<tnAsset>& hande, const UUID& uuid)
	{
		hande = AssetHandle<tnAsset>(AssetManager::AssetCreation::BaseAsset<tnAsset>(path, uuid), AssetLoadingPriority::LoadingPriority_None);
	}

	void Renderer::AcquireBaseAssets()
	{
		FE_PROFILER_FUNC();

		auto& base_assets = Project::GetInstance()->BaseAssets;

		MakeHandle("base_assets/textures/Default_Texture.png",	BaseAssets.Textures.Default,		base_assets.Textures.Default);
		MakeHandle("base_assets/textures/FlatWhite.png",		BaseAssets.Textures.FlatWhite,		base_assets.Textures.FlatWhite);
		MakeHandle("base_assets/shaders/Base2DShader.glsl",		BaseAssets.Shaders.Base2D,			base_assets.Shaders.Base2D);
		MakeHandle("base_assets/shaders/Base3DShader.glsl",		BaseAssets.Shaders.Base3D,			base_assets.Shaders.Base3D);
		MakeHandle("base_assets/shading_models/Default.fesm",	BaseAssets.ShadingModels.Default,	base_assets.ShadingModels.Default);
		MakeHandle("Default.femat",								BaseAssets.Materials.Default,		base_assets.Materials.Default);
		// "_Default.femat" is a dummy path

		TextureLoader::LoadTexture("base_assets/textures/Default_Texture.png", BaseAssets.Textures.Default.Use());
		TextureLoader::LoadTexture("base_assets/textures/FlatWhite.png"      , BaseAssets.Textures.FlatWhite.Use());

		ShaderLoader::LoadShader("base_assets/shaders/Base2DShader.glsl", BaseAssets.Shaders.Base2D.Use());
		ShaderLoader::LoadShader("base_assets/shaders/Base3DShader.glsl", BaseAssets.Shaders.Base3D.Use());

		bool succes = ShadingModel::DeserializeFromFile(BaseAssets.ShadingModels.Default.GetID(), "base_assets/shading_models/Default.fesm");
		FE_CORE_ASSERT(succes, "Failed to load default shading model");

		BaseAssets.Materials.Default.Use().MakeMaterial(BaseAssets.ShadingModels.Default.Observe());

		UploadBaseAssetsToGPU(Renderer::GetActiveGDItype());

		Renderer2D::s_Data.BaseShader = Renderer::BaseAssets.Shaders.Base2D;
		Renderer2D::s_Data.Batch.Textures[0] = Renderer::BaseAssets.Textures.FlatWhite.GetID();
	}

	void Renderer::UploadBaseAssetsToGPU(GDIType GDI)
	{
		FE_PROFILER_FUNC();

		BaseAssets.Textures.Default.Use().CreateGDITexture2D(GDI);
		BaseAssets.Textures.FlatWhite.Use().CreateGDITexture2D(GDI);

		ShaderLoader::CompileShader(GDI, BaseAssets.Shaders.Base2D.Use());
		ShaderLoader::CompileShader(GDI, BaseAssets.Shaders.Base3D.Use());
		
	}

	void Renderer::SetAPI(GDIType GDI)
	{
		FE_PROFILER_FUNC();
		
		FE_CORE_ASSERT(s_DeviceAPIs.find(GDI) != s_DeviceAPIs.end(), "API not created!");
		
		s_ActiveGDI = GDI;

		auto& deviceAPI = s_DeviceAPIs.at(GDI);

		RenderCommands::SetAPI(deviceAPI.get());

		Renderer2D::Init();
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

	void Renderer::RenderScene(const AssetObserver<Scene>& scene, Framebuffer& framebuffer)
	{
		auto cameraEntity = scene.GetCoreComponent().GameplayWorld->GetEntityWithPrimaryCamera();
		auto& cameraComponent = cameraEntity.Get<CCamera>();
		auto& camera = cameraComponent.Camera;
		auto cameraTransform = cameraEntity.GetTransformHandle().Global();
		cameraTransform.Scale = { 1.f,1.f,1.f };
		cameraTransform = cameraTransform + cameraComponent.Offset;
		RenderScene(scene, camera, cameraTransform, framebuffer);
	}

	void Renderer::RenderScene(const AssetObserver<Scene>& scene, const Camera& camera, const Transform& cameraTransform, Framebuffer& framebuffer)
	{
		FE_PROFILER_FUNC();

		BeginScene(camera, cameraTransform, framebuffer);

		Renderer2D::RenderScene(scene, camera, cameraTransform, framebuffer);

		auto& registry = scene.GetCoreComponent().GameplayWorld->GetRegistry();
		void* VPmatrixPtr = (void*)glm::value_ptr(s_SceneData->VPMatrix);
		auto GDI = Renderer::GetActiveGDItype();


		//auto viewMeshes = registry.view<CRenderMesh, CTransformGlobal>();
		//for (auto ID : viewMeshes)
		//{
		//	auto [render_mesh_component, transform_component] = viewMeshes.get(ID);
		//	if (!render_mesh_component.RenderMesh.IsValid())
		//		continue;
		//
		//	glm::mat4 modelTransform = transform_component.GetRef().GetMatrix();
		//	void* modelTransformPtr = (void*)glm::value_ptr(modelTransform);
		//
		//	auto render_mesh_observer = render_mesh_component.RenderMesh.Observe();
		//
		//	auto& render_mesh_core = render_mesh_observer.GetCoreComponent();
		//	auto material_observer = AssetObserver<Material>(render_mesh_core.MaterialID);
		//	auto shaderID = AssetObserver<ShadingModel>(material_observer.GetCoreComponent().ShadingModelID).GetCoreComponent().ShaderID;
		//	{
		//		auto shader_observer = AssetObserver<Shader>(shaderID);
		//
		//		shader_observer.Bind(GDI);
		//		shader_observer.UploadUniform(GDI, Uniform("u_ViewProjection", ShaderData::Type::Mat4), VPmatrixPtr);
		//		shader_observer.UploadUniform(GDI, Uniform("u_ModelTransform", ShaderData::Type::Mat4), modelTransformPtr);
		//		shader_observer.UploadUniform(GDI, Uniform("u_EntityID", ShaderData::Type::UInt), &ID);
		//	}
		//
		//	AssetObserver<Mesh>(render_mesh_core.MeshID).Draw(material_observer);
		//}

		auto viewViewMeshes = registry.view<CRenderMeshView, CTransformGlobal>();
		for (auto ID : viewViewMeshes)
		{
			auto [renderViewMesh_component, transform_component] = viewViewMeshes.get(ID);
			if (!renderViewMesh_component.Material.IsValid())
				continue;
			if (!renderViewMesh_component.Mesh.IsValid())
				continue;

			glm::mat4 modelTransform = transform_component.GetRef().GetMatrix();
			void* modelTransformPtr = (void*)glm::value_ptr(modelTransform);

			auto material_observer = renderViewMesh_component.Material.Observe();
			auto mesh_observer = renderViewMesh_component.Mesh.Observe();

			auto y = material_observer.GetCoreComponent().ShadingModelID;
			auto x = AssetObserver<ShadingModel>(y);
			auto shaderID = x.GetCoreComponent().ShaderID;

			{
				auto shader_observer = AssetObserver<Shader>(shaderID);

				shader_observer.Bind(GDI);
				shader_observer.UploadUniform(GDI, Uniform("u_ViewProjection", ShaderData::Type::Mat4), VPmatrixPtr);
				shader_observer.UploadUniform(GDI, Uniform("u_ModelTransform", ShaderData::Type::Mat4), modelTransformPtr);
				shader_observer.UploadUniform(GDI, Uniform("u_EntityID", ShaderData::Type::UInt), &ID);
			}

			mesh_observer.Draw(material_observer);
		}


		framebuffer.Unbind();

		GLenum error = glGetError();

		if (error)
			FE_LOG_CORE_INFO("{0}", error);

		EndScene();
	}

	void Renderer::BeginScene(const glm::mat4& projection, const glm::mat4& view, Framebuffer& framebuffer)
	{
		FE_PROFILER_FUNC();

		framebuffer.Bind();
		
		RenderCommands::Clear();
		RenderCommands::SetClearColor({ 0.1, 0.1, 0.1, 1 });

		int attachmentIndex = framebuffer.GetColorAttachmentIndex("EntityID");
		framebuffer.ClearAttachment(attachmentIndex, (uint32_t)NullEntityID);

		switch (s_ActiveGDI.Value)
		{
		case GDIType::OpenGL:
			s_SceneData->VPMatrix = projection * glm::inverse(view);
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
	}

	void Renderer::Draw(const Ref<VertexBuffer>& vertexBuffer, const AssetObserver<Material>& materialObserver, const glm::mat4& transform)
	{
		Draw(vertexBuffer, materialObserver, transform, s_SceneData->VPMatrix);
	}

	void Renderer::Draw(
		const Ref<VertexBuffer>& vertexBuffer,
		const AssetObserver<Material>& materialObserver,
		const glm::mat4& transform,
		const glm::mat4& VPMatrix)
	{
		FE_PROFILER_FUNC();

		auto& material_core = materialObserver.GetCoreComponent();
		auto sm_observer = AssetObserver<ShadingModel>(material_core.ShadingModelID);
		auto& sm_core = sm_observer.GetCoreComponent();
		auto shaderUser = AssetUser<Shader>(sm_core.ShaderID);

		shaderUser.Bind(s_ActiveGDI);

		shaderUser.UploadUniform(
			s_ActiveGDI,
			Uniform("u_ViewProjection", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(VPMatrix)
		);
		shaderUser.UploadUniform(
			s_ActiveGDI,
			Uniform("u_Transform", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(transform)
		);

		for (const auto& uniform : sm_core.Uniforms)
		{
			auto dataPointer = materialObserver.GetUniformValuePtr(material_core, uniform);
			shaderUser.UploadUniform(s_ActiveGDI, uniform, dataPointer);
		}

		{
			uint32_t rendererTextureSlot = 0;
			auto shaderTextureSlotsIt = sm_core.TextureSlots.begin();

			for (const auto& texture : material_core.Textures)
			{
				shaderUser.BindTextureSlot(s_ActiveGDI, *shaderTextureSlotsIt++, rendererTextureSlot);

				if (texture)
				{
					AssetUser<Texture2D>(texture).Bind(s_ActiveGDI, rendererTextureSlot++);
				}
				else
				{
					FE_LOG_CORE_WARN("Uninitialized texture!");
					Renderer::BaseAssets.Textures.Default.Use().Bind(s_ActiveGDI, rendererTextureSlot++);
					continue;
				}
			}
		}

		vertexBuffer->Bind();

		RenderCommands::DrawIndexed(vertexBuffer.get());
	}
} 