#include "FE_pch.h"
#include "Renderer.h"

#include <glad\glad.h>

namespace fe
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
	std::unordered_map<RenderCommands::APItype, Scope<RendererAPI>> Renderer::s_RenderingAPIs = std::unordered_map<RenderCommands::APItype, Scope<RendererAPI>>();

	void Renderer::Init()
	{
		FE_PROFILER_FUNC();
	}

	void Renderer::SetAPI(RenderCommands::APItype API)
	{
		FE_PROFILER_FUNC();
		
		if (!s_RenderingAPIs.count(API))
		{
			CreateAPI(API);
		}
		
		RenderCommands::SetAPI(s_RenderingAPIs.at(API).get(), API);

		Texture::s_DefaultTexture = Texture2D::Create("assets/textures/Default_Texture.png");
	}

	void Renderer::CreateAPI(RenderCommands::APItype API)
	{
		FE_PROFILER_FUNC();

		if (s_RenderingAPIs.count(API))
		{
			FE_CORE_ASSERT(false, "This RendererAPI is already created!");
			return;
		}
		s_RenderingAPIs[API] = RenderCommands::CreateAPI(API);
		s_RenderingAPIs.at(API)->Init();
	}

	void Renderer::BeginScene(OrtographicCamera& camera)
	{
		FE_PROFILER_FUNC();
		s_SceneData->VPMatrix = camera.GetViewProjectionMatrix();
		fe::RenderCommands::Clear();
		fe::RenderCommands::SetClearColor({ 0.1, 0.1, 0.1, 1 });
	}

	void Renderer::EndScene()
	{
		FE_PROFILER_FUNC();
	}

	void Renderer::Submit(
		const Ref<VertexArray>& vertexArray,
		const Ref<MaterialInstance>& materialInstance,
		const glm::mat4& transform)
	{
		FE_PROFILER_FUNC();

		// TO DO: adding into a queue for future collective draw at the end of main loop
		const Ref<Shader>& shader = materialInstance->GetMaterial()->GetShader();

		shader->Bind();

		shader->UploadUniform(
			Uniform("u_ViewProjection", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(s_SceneData->VPMatrix)
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

			for each (auto texture in materialInstance->GetTextures())
			{
				shader->BindTextureSlot(*shaderTextureSlotsIt++, rendererTextureSlot);
				//shader->UploadUniform(Uniform(textureSlotsIt++->GetName(), ShaderData::Type::Int), &);
				if (!texture.get())
				{
					FE_CORE_ASSERT(false, "Uninitialized texture!");
					Texture::s_DefaultTexture->Bind(rendererTextureSlot++);
					continue;
				}

				texture->Bind(rendererTextureSlot++);
			}
		}

		vertexArray->Bind();

		RenderCommands::DrawIndexed(vertexArray);
	}
}