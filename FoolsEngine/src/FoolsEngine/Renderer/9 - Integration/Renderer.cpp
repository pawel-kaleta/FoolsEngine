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

#include <glad\glad.h>

#include <glm\gtc\type_ptr.hpp>

namespace fe
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
	decltype(Renderer::BaseAssets) Renderer::BaseAssets;
	GDIType Renderer::s_ActiveGDI = GDIType::none;
	std::unordered_map<GDIType, Scope<DeviceAPI>> Renderer::s_DeviceAPIs;

	void Renderer::Startup()
	{
		FE_PROFILER_FUNC();

		CreateBaseAssets();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::CreateBaseAssets()
	{
		FE_PROFILER_FUNC();

		//////////////
		//
		// TO DO:
		//
		// MOVE THIS TO PROJECT CREATION
		//
		///////////////

		{
			auto id1 = AssetManager::CreateAsset<Texture2D>("assets/textures/Default_Texture.png");
			auto& handle1 = BaseAssets.Textures.Default;
			new (&handle1) AssetHandle<Texture2D>(id1, LoadingPriority_Critical);

			auto id2 = AssetManager::CreateAsset<Texture2D>("assets/textures/FlatWhite.png");
			auto& handle2 = BaseAssets.Textures.FlatWhite;
			new (&handle2) AssetHandle<Texture2D>(id2, LoadingPriority_Critical);
		}

		{
			auto id1 = AssetManager::CreateAsset<Shader>("assets/shaders/Base2DShader.glsl");
			auto& handle1 = BaseAssets.Shaders.Base2D;
			new (&handle1) AssetHandle<Shader>(id1, LoadingPriority_Critical);

			auto id2 = AssetManager::CreateAsset<Shader>("assets/shaders/Base3DShader.glsl");
			auto& handle2 = BaseAssets.Shaders.Base3D;
			new (&handle2) AssetHandle<Shader>(id2, LoadingPriority_Critical);
		}

		{
			auto id1 = AssetManager::CreateAsset<ShadingModel>("assets/shading_models/Default.femat");
			auto& handle1 = BaseAssets.ShadingModels.Default;
			new (&handle1) AssetHandle<ShadingModel>(id1, LoadingPriority_Critical);
			auto shading_model_user = handle1.Use();

			// TO DO: read from a file
			shading_model_user.MakeShadingModel(
				BaseAssets.Shaders.Base3D.GetID(),
				{
					Uniform("u_BaseColor", ShaderData::Type::Float3),
					Uniform("u_Roughness", ShaderData::Type::Float),
					Uniform("u_Metalness", ShaderData::Type::Float),
					Uniform("u_AO"       , ShaderData::Type::Float)
				},
				{
					ShaderTextureSlot("u_BaseColorMap", TextureData::Type::Texture2D),
					ShaderTextureSlot("u_RoughnessMap", TextureData::Type::Texture2D),
					ShaderTextureSlot("u_MetalnessMap", TextureData::Type::Texture2D),
					ShaderTextureSlot("u_AOMap"       , TextureData::Type::Texture2D),
					ShaderTextureSlot("u_NormalMap"   , TextureData::Type::Texture2D)
				}
			);
		}

		{
			auto id1 = AssetManager::CreateAsset<Material>("DefaultMaterial");
			auto& handle1 = BaseAssets.Materials.Default;
			new (&handle1) AssetHandle<Material>(id1, LoadingPriority_Critical);
			auto material_user = handle1.Use();

			// TO DO: save to file
			material_user.MakeMaterial(BaseAssets.ShadingModels.Default.Observe());
			*(glm::vec3*)material_user.GetUniformValuePtr(material_user.GetCoreComponent(), "u_BaseColor") = {1.0f, 1.0f, 1.0f};
		}
	}

	void Renderer::UploadBaseAssetsToGPU(GDIType GDI)
	{
		BaseAssets.Textures.Default.Use().CreateGDITexture2D(GDI);
		BaseAssets.Textures.FlatWhite.Use().CreateGDITexture2D(GDI);

		auto assetUser1 = BaseAssets.Shaders.Base2D.Use();
		ShaderLoader::CompileShader(GDI, assetUser1);

		auto assetUser2 = BaseAssets.Shaders.Base3D.Use();
		ShaderLoader::CompileShader(GDI, assetUser2);
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

		UploadBaseAssetsToGPU(GDI);
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
		auto viewMeshes = registry.view<CRenderMesh, CTransformGlobal>();
		void* VPmatrixPtr = (void*)glm::value_ptr(s_SceneData->VPMatrix);

		auto GDI = Renderer::GetActiveGDItype();

		for (auto ID : viewMeshes)
		{
			auto [render_mesh_component, transform_component] = viewMeshes.get(ID);
			if (!render_mesh_component.RenderMesh.IsValid())
				continue;

			glm::mat4 modelTransform = transform_component.GetRef().GetMatrix();
			void* modelTransformPtr = (void*)glm::value_ptr(modelTransform);

			auto render_mesh_observer = render_mesh_component.RenderMesh.Observe();

			auto& render_mesh_core = render_mesh_observer.GetCoreComponent();
			auto material_observer = AssetObserver<Material>(render_mesh_core.MaterialID);
			auto shaderID = AssetObserver<ShadingModel>(material_observer.GetCoreComponent().ShadingModelID).GetCoreComponent().ShaderID;
			{
				auto shader_observer = AssetObserver<Shader>(shaderID);

				shader_observer.Bind(GDI);
				shader_observer.UploadUniform(GDI, Uniform("u_ViewProjection", ShaderData::Type::Mat4), VPmatrixPtr);
				shader_observer.UploadUniform(GDI, Uniform("u_ModelTransform", ShaderData::Type::Mat4), modelTransformPtr);
				shader_observer.UploadUniform(GDI, Uniform("u_EntityID", ShaderData::Type::UInt), &ID);
			}

			AssetObserver<Mesh>(render_mesh_core.MeshID).Draw(material_observer);
		}

		framebuffer.Unbind();

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

		switch (s_ActiveGDI)
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