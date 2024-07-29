#include "FE_pch.h"
#include "Renderer.h"

#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"
#include "FoolsEngine\Renderer\8 - Render\Renderer2D.h"
#include "FoolsEngine\Renderer\3 - Representation\MaterialInstance.h"
#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"

#include "FoolsEngine\Assets\AssetManager.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"

#include "FoolsEngine\Assets\Loaders\ShaderLoader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\OpenGL\OpenGLShader.h"

#include <glad\glad.h>

namespace fe
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
	GDIType Renderer::s_ActiveGDI = GDIType::none;
	std::unordered_map<GDIType, Scope<DeviceAPI>> Renderer::s_DeviceAPIs;

	void Renderer::Init()
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

		{
			auto id1 = AssetManager::NewAsset<Texture2D>((AssetID)BaseAssets::Textures2D::Default);
			FE_CORE_ASSERT(id1 == (AssetID)BaseAssets::Textures2D::Default, "Failed to create base asset with requested ID");

			TextureLoader::LoadTexture("assets/textures/Default_Texture.png", AssetHandle<Texture2D>(id1).Use());

			// ----------------

			auto id2 = AssetManager::NewAsset<Texture2D>((AssetID)BaseAssets::Textures2D::FlatWhite);
			FE_CORE_ASSERT(id2 == (AssetID)BaseAssets::Textures2D::FlatWhite, "Failed to create base asset with requested ID");
			auto whiteTextureUser = AssetHandle<Texture2D>(id2).Use();

			uint32_t* whiteTextureData = new uint32_t(0xffffffff);
			Texture2DBuilder textureBuilder;
			textureBuilder
				.SetDataPtr(whiteTextureData)
				.SetResolution(1, 1)
				.SetType(TextureData::Type::Texture2D)
				.SetUsage(TextureData::Usage::Map_Albedo)
				.SetComponents(TextureData::Components::RGBA_F)
				.SetFormat(TextureData::Format::RGBA_FLOAT_8);

			textureBuilder.Create(whiteTextureUser);
			whiteTextureUser.GetFilepath().Filepath = "FlatWhite";
		}

		{
			auto id1 = AssetManager::NewAsset<Shader>((AssetID)BaseAssets::Shaders::Default2D);
			FE_CORE_ASSERT(id1 == (AssetID)BaseAssets::Shaders::Default2D, "Failed to create base asset with requested ID");

			auto assetUser1 = AssetHandle<Shader>(id1).Use();
			ShaderLoader::LoadShader("assets/shaders/Base2DShader.glsl", assetUser1);

			// ----------------

			auto id2 = AssetManager::NewAsset<Shader>((AssetID)BaseAssets::Shaders::Default3D);
			FE_CORE_ASSERT(id2 == (AssetID)BaseAssets::Shaders::Default3D, "Failed to create base asset with requested ID");

			auto assetUser2 = AssetHandle<Shader>(id2).Use();
			ShaderLoader::LoadShader("assets/shaders/Base3DShader.glsl", assetUser2);
		}

		{
			auto id1 = AssetManager::NewAsset<Material>((AssetID)BaseAssets::Materials::Default3D);
			FE_CORE_ASSERT(id1 == (AssetID)BaseAssets::Materials::Default3D, "Failed to create base asset with requested ID");

			Material::MakeMaterial(
				AssetHandle<Material>(id1).Use(),
				AssetHandle<Shader>((AssetID)BaseAssets::Shaders::Default3D),
				{
					Uniform("u_Albedo"   , ShaderData::Type::Float3),
					Uniform("u_Roughness", ShaderData::Type::Float),
					Uniform("u_Metalness", ShaderData::Type::Float),
					Uniform("u_AO"       , ShaderData::Type::Float)
				},
				{
					ShaderTextureSlot("u_AlbedoMap"   , TextureData::Type::Texture2D),
					ShaderTextureSlot("u_RoughnessMap", TextureData::Type::Texture2D),
					ShaderTextureSlot("u_MetalnessMap", TextureData::Type::Texture2D),
					ShaderTextureSlot("u_AOMap"       , TextureData::Type::Texture2D),
					ShaderTextureSlot("u_NormalMap"   , TextureData::Type::Texture2D)
				}
				);
		}

		{
			auto id1 = AssetManager::NewAsset<MaterialInstance>((AssetID)BaseAssets::MaterialInstances::Default3D);
			FE_CORE_ASSERT(id1 == (AssetID)BaseAssets::MaterialInstances::Default3D, "Failed to create base asset with requested ID");

			auto miUser = AssetHandle<MaterialInstance>(id1).Use();
			MaterialInstance::MakeMaterialInstance(miUser);
			miUser.Init(AssetHandle<Material>((AssetID)BaseAssets::Materials::Default3D));
		}
	}

	void Renderer::UploadBaseAssetsToGPU(GDIType GDI)
	{
		{
			auto id1 = (AssetID)BaseAssets::Textures2D::Default;
			auto textureUser = AssetHandle<Texture2D>(id1).Use();
			textureUser.CreateGDITexture2D(GDI);
			
			// ----------------

			auto id2 = (AssetID)BaseAssets::Textures2D::FlatWhite;
			auto textureUser2 = AssetHandle<Texture2D>(id2).Use();
			textureUser2.CreateGDITexture2D(GDI);
		}

		{
			auto id1 = (AssetID)BaseAssets::Shaders::Default2D;
			auto assetUser1 = AssetHandle<Shader>(id1).Use();
			ShaderLoader::CompileShader(GDI, assetUser1);

			// ----------------

			auto id2 = (AssetID)BaseAssets::Shaders::Default3D;
			auto assetUser2 = AssetHandle<Shader>(id2).Use();
			ShaderLoader::CompileShader(GDI, assetUser2);
		}
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

	void Renderer::BeginScene(Camera& camera)
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(false, "3D rendering not supported yet!");
		RenderCommands::Clear();
		RenderCommands::SetClearColor({ 0.1, 0.1, 0.1, 1 });
	}

	void Renderer::EndScene()
	{
		FE_PROFILER_FUNC();
	}

	void Renderer::Draw(
		const Ref<VertexBuffer>& vertexBuffer,
		AssetHandle<MaterialInstance> materialInstance,
		const glm::mat4& transform,
		const glm::mat4& VPMatrix)
	{
		FE_PROFILER_FUNC();

		auto miObserver = materialInstance.Observe();
		auto materialObserver = miObserver.GetMaterial().Observe();

		auto& shaderUser = materialObserver.GetShader().Use();

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

		for (auto& uniform : materialObserver.GetUniforms())
		{
			void* dataPointer = miObserver.GetUniformValuePtr(uniform);
			shaderUser.UploadUniform(s_ActiveGDI, uniform, dataPointer);
		}

		{
			uint32_t rendererTextureSlot = 0;
			auto shaderTextureSlotsIt = materialObserver.GetTextureSlots().begin();

			for (auto& texture : miObserver.GetTextures())
			{
				shaderUser.BindTextureSlot(s_ActiveGDI, *shaderTextureSlotsIt++, rendererTextureSlot);
				
				if (!texture.IsValid())
				{
					FE_LOG_CORE_WARN("Uninitialized texture!");
					AssetHandle<Texture2D>((AssetID)BaseAssets::Textures2D::Default).Use().Bind(s_ActiveGDI, rendererTextureSlot++);
					continue;
				}

				texture.Use().Bind(s_ActiveGDI, rendererTextureSlot++);
			}
		}

		vertexBuffer->Bind();

		RenderCommands::DrawIndexed(vertexBuffer.get());
	}
} 