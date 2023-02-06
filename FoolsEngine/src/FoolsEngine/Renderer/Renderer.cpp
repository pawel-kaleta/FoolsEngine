#include "FE_pch.h"
#include "Renderer.h"

#include <glad\glad.h>

namespace fe
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
	GDIType Renderer::s_ActiveGDI = GDIType::none;
	std::unordered_map<GDIType, Renderer::GDIFixedSystems> Renderer::s_GDIFixedSystems = std::unordered_map<GDIType, Renderer::GDIFixedSystems>();

	void Renderer::Init()
	{
		FE_PROFILER_FUNC();
	}

	void Renderer::SetAPI(GDIType GDI)
	{
		FE_PROFILER_FUNC();
		
		FE_CORE_ASSERT(s_GDIFixedSystems.find(GDI) != s_GDIFixedSystems.end(), "API not created!");
		
		s_ActiveGDI = GDI;

		GDIFixedSystems& systems = s_GDIFixedSystems.at(GDI);

		RenderCommands::SetAPI(systems.RendererAPI.get());
		ShaderLibrary::SetActiveInstance(systems.ShaderLib.get());
		TextureLibrary::SetActiveInstance(systems.TextureLib.get());
		MaterialLibrary::SetActiveInstance(systems.MaterialLib.get());
		Renderer2D::Init();
	}

	void Renderer::CreateAPI(GDIType GDI)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(s_GDIFixedSystems.find(GDI) == s_GDIFixedSystems.end(), "API already created!");
		
		GDIFixedSystems& systems = s_GDIFixedSystems[GDI];

		systems.GDI = GDI;
		systems.RendererAPI = RenderCommands::CreateAPI(GDI);
		systems.ShaderLib.reset(new ShaderLibrary());
		systems.TextureLib.reset(new TextureLibrary());
		systems.MaterialLib.reset(new MaterialLibrary());
	}

	void Renderer::InitAPI(GDIType GDI)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(s_GDIFixedSystems.find(GDI) != s_GDIFixedSystems.end(), "API not created!");

		GDIFixedSystems& systems = s_GDIFixedSystems.at(GDI);

		systems.RendererAPI->Init();

		switch (GDI)
		{
		case GDIType::OpenGL:
			systems.ShaderLib->IAdd(Shader::Create("assets/shaders/Flat_Color_Shader.glsl", GDI));
			systems.ShaderLib->IAdd(Shader::Create("assets/shaders/Basic_Texture_Shader.glsl", GDI));
			break;
		default:
			FE_CORE_ASSERT(false, "Uknown GDIType!");
		}

		systems.TextureLib->IAdd(Texture2D::Create("assets/textures/Default_Texture.png", GDI));

		systems.MaterialLib->IAdd(
			Ref<Material>(
				new Material(
					"Flat_Color_Material",
					systems.ShaderLib->IGet("Flat_Color_Shader"),
					{
						{ "u_Color", ShaderData::Type::Float4 }
					},
					{}
				)
			)
		);

		systems.MaterialLib->IAdd(
			Ref<Material>(
				new fe::Material(
					"Basic_Texture_Material",
					systems.ShaderLib->IGet("Basic_Texture_Shader"),
					{},
					{
						{ "u_Texture", TextureType::Texture2D }
					}
				)
			)
		);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommands::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		FE_PROFILER_FUNC();
		s_SceneData->VPMatrix = camera.GetViewProjectionMatrix();
		RenderCommands::Clear();
		RenderCommands::SetClearColor({ 0.1, 0.1, 0.1, 1 });
	}

	void Renderer::EndScene()
	{
		FE_PROFILER_FUNC();
	}

	void Renderer::Draw(
		const Ref<VertexArray>& vertexArray,
		const Ref<MaterialInstance>& materialInstance,
		const glm::mat4& transform,
		const glm::mat4& VPMatrix)
	{
		FE_PROFILER_FUNC();

		const Ref<Shader>& shader = materialInstance->GetMaterial()->GetShader();

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
				
				if (!texture.get())
				{
					//FE_CORE_ASSERT(false, "Uninitialized texture!");
					TextureLibrary::Get("Default_Texture")->Bind(rendererTextureSlot++);
					continue;
				}

				texture->Bind(rendererTextureSlot++);
			}
		}

		vertexArray->Bind();

		RenderCommands::DrawIndexed(vertexArray);
	}
}