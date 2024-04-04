#include "FE_pch.h"
#include "Renderer.h"

#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"
#include "FoolsEngine\Renderer\8 - Render\Renderer2D.h"
#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"

#include <glad\glad.h>

namespace fe
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
	GDIType Renderer::s_ActiveGDI = GDIType::none;
	std::unordered_map<GDIType, Scope<DeviceAPI>> Renderer::s_DeviceAPIs;

	void Renderer::Init()
	{
		FE_PROFILER_FUNC();
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

		switch (GDI)
		{
		case GDIType::OpenGL:
			// markmark
			//systems.ShaderLib->IAdd(Shader::Create("assets/shaders/Base2DShader.glsl", GDI));
			//systems.ShaderLib->IAdd(Shader::Create("assets/shaders/Base3DShader.glsl", GDI));
			break;
		default:
			FE_CORE_ASSERT(false, "Uknown GDIType!");
		}

		// markmark
		//systems.TextureLib->IAdd(Texture2D::Create("assets/textures/Default_Texture.png", GDI, TextureData::Usage::Map_Albedo));

		TextureBuilder textureBuilder;
		textureBuilder
			.SetGDI(GDI)
			.SetHeight(1)
			.SetWidth(1)
			.SetName("WhiteTexture")
			.SetType(TextureData::Type::Texture2D)
			.SetUsage(TextureData::Usage::Map_Albedo)
			.SetComponents(TextureData::Components::RGBA_F)
			.SetFormat(TextureData::Format::RGBA_FLOAT_8);

		Texture* whiteTexture = textureBuilder.Create();
		uint32_t whiteTextureData = 0xffffffff;
		whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		// markmark
		//systems.TextureLib->IAdd(whiteTexture);
		
		// markmark
		/*
		auto defaultMaterial3D = new Material(
			"Default3DMaterial",
			systems.ShaderLib->IGet("Base3DShader"),
			{
				//Uniform("u_ViewProjection", ShaderData::Type::Mat4),
				//Uniform("u_EntityID"      , ShaderData::Type::UInt),
				//Uniform("u_Position"      , ShaderData::Type::Float3),
				Uniform("u_Albedo"        , ShaderData::Type::Float3),
				Uniform("u_Roughness"     , ShaderData::Type::Float),
				Uniform("u_Metalness"     , ShaderData::Type::Float),
				Uniform("u_AO"            , ShaderData::Type::Float)
			},
			{
				ShaderTextureSlot("u_AlbedoMap"   , TextureData::Type::Texture2D),
				ShaderTextureSlot("u_RoughnessMap", TextureData::Type::Texture2D),
				ShaderTextureSlot("u_MetalnessMap", TextureData::Type::Texture2D),
				ShaderTextureSlot("u_AOMap"       , TextureData::Type::Texture2D),
				ShaderTextureSlot("u_NormalMap"   , TextureData::Type::Texture2D)
			}
		);
		*/

		// markmark
		// systems.MaterialLib->IAdd(defaultMaterial3D);

		// markmark
		// systems.MaterialInstanceLib->IAdd(Ref<MaterialInstance>(new MaterialInstance(
		//	defaultMaterial3D,
		//	defaultMaterial3D->GetName() + "_Instance"
		//)));
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
		const AssetHandle<MaterialInstance>& materialInstance,
		const glm::mat4& transform,
		const glm::mat4& VPMatrix)
	{
		FE_PROFILER_FUNC();

		auto& shader = materialInstance->GetMaterial()->GetShader();

		shader->Bind();

		shader->UploadUniform(
			Uniform("u_ViewProjection", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(VPMatrix)
		);
		shader->UploadUniform(
			Uniform("u_Transform", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(transform)
		);

		for (auto& uniform : materialInstance->GetMaterial()->GetUniforms())
		{
			void* dataPointer = materialInstance->GetUniformValuePtr(uniform);
			shader->UploadUniform(uniform, dataPointer);
		}

		{
			uint32_t rendererTextureSlot = 0;
			auto shaderTextureSlotsIt = materialInstance->GetMaterial()->GetTextureSlots().begin();

			for (auto& texture : materialInstance->GetTextures())
			{
				shader->BindTextureSlot(*shaderTextureSlotsIt++, rendererTextureSlot);
				
				if (!texture)
				{
					//FE_CORE_ASSERT(false, "Uninitialized texture!");
					// markmark
					//TextureLibrary::Get("Default_Texture")->Bind(rendererTextureSlot++);
					continue;
				}

				texture->Bind(rendererTextureSlot++);
			}
		}

		vertexBuffer->Bind();

		RenderCommands::DrawIndexed(vertexBuffer.get());
	}
}