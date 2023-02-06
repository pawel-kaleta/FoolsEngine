#include "FE_pch.h"
#include "Renderer2D.h"
#include "Renderer.h"

namespace fe
{
	Scope<Renderer2D::Renderer2DData> Renderer2D::s_Data = nullptr;

	void Renderer2D::Init()
	{
		s_Data = CreateScope<Renderer2DData>();

		s_Data->QuadVertexArray = VertexArray::Create();
		s_Data->QuadVertexArray->Bind();

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,  0.0f, 1.0f
		};
		Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ ShaderData::Type::Float3, "a_Position" },
			{ ShaderData::Type::Float2, "a_TexCoord" }
		});
		s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

		uint32_t squareIndicies[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndicies, 6);
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

		s_Data->BaseShader = Shader::Create("assets/shaders/Base2DShader.glsl");

		s_Data->BaseShaderTextureSlot = ShaderTextureSlot("u_Texture", TextureType::Texture2D);

		s_Data->WhiteTexture = Texture2D::Create("Base2DTexture", 1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));


		s_Data->FlatColorMaterialInstance = Ref<MaterialInstance>(new MaterialInstance(MaterialLibrary::Get("Flat_Color_Material")));
		s_Data->BasicTextureMaterialInstance = Ref<MaterialInstance>(new MaterialInstance(MaterialLibrary::Get("Basic_Texture_Material")));
	}

	void Renderer2D::BeginScene(OrthographicCamera& camera)
	{
		RenderCommands::Clear();
		RenderCommands::SetClearColor({ 0.1, 0.1, 0.1, 1 });

		s_Data->VPMatrix = camera.GetViewProjectionMatrix();

		s_Data->BaseShader->Bind();
		s_Data->BaseShader->UploadUniform(
			Uniform("u_ViewProjection", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(s_Data->VPMatrix)
		);

		s_Data->WhiteTexture->Bind(0);
	}

	void Renderer2D::EndScene()
	{
		for (auto& quad : s_Data->UniqueQuads)
		{
			DrawUniqueQuad(quad);
		}
	}

	void Renderer2D::DrawQuad(const Quad& quad)
	{
		if (quad.m_MaterialActive)
		{
			s_Data->UniqueQuads.push_back(quad);
			return;
		}

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), glm::vec3(quad.Position, (float)quad.Layer))
			*
			glm::scale(glm::mat4(1.0f), glm::vec3(quad.Size, 1.0f));

		s_Data->BaseShader->UploadUniform(
			Uniform("u_Transform", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(transform)
		);

		s_Data->BaseShader->UploadUniform(
			Uniform("u_Color", ShaderData::Type::Float4),
			(void*)glm::value_ptr(quad.Color)
		);
		 
		uint32_t rendererTextureSlot = 0;
		if (quad.m_TextureActive)
		{
			s_Data->BaseShader->UploadUniform(
				Uniform("u_TextureTilingFactor", ShaderData::Type::Float),
				(void*) &(quad.TextureTilingFactor)
			);

			rendererTextureSlot = 1;
			quad.m_Texture->Bind(rendererTextureSlot);
		}
		
		s_Data->BaseShader->BindTextureSlot(s_Data->BaseShaderTextureSlot, rendererTextureSlot);
		
		s_Data->QuadVertexArray->Bind();
		RenderCommands::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawUniqueQuad(const Quad& quad)
	{
		const Ref<MaterialInstance>& MI = quad.m_MaterialInstance;
		const Ref<Shader>& shader = MI->GetMaterial()->GetShader();

		shader->Bind();

		shader->UploadUniform(
			Uniform("u_ViewProjection", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(s_Data->VPMatrix)
		);

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), glm::vec3(quad.Position, quad.Layer))
			*
			glm::scale(glm::mat4(1.0f), glm::vec3(quad.Size, 1.0f));

		shader->UploadUniform(
			Uniform("u_Transform", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(transform)
		);

		for (auto& uniform : MI->GetMaterial()->GetUniforms())
		{
			void* dataPointer = MI->GetUniformValuePtr(uniform);
			shader->UploadUniform(uniform, dataPointer);
		}

		{
			uint32_t rendererTextureSlot = 1;
			auto shaderTextureSlotsIt = MI->GetMaterial()->GetTextureSlots().begin();

			for (auto& texture : MI->GetTextures())
			{
				shader->BindTextureSlot(*shaderTextureSlotsIt++, rendererTextureSlot);

				if (!texture.get())
				{
					TextureLibrary::Get("Default_Texture")->Bind(rendererTextureSlot++);
					continue;
				}

				texture->Bind(rendererTextureSlot++);
			}
		}

		s_Data->QuadVertexArray->Bind();

		RenderCommands::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		Ref<MaterialInstance>& MI = s_Data->FlatColorMaterialInstance;
		MI->SetUniformValue(MI->GetMaterial()->GetUniforms()[0], (void*)glm::value_ptr(color));

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		Renderer::Draw(s_Data->QuadVertexArray, MI, transform, s_Data->VPMatrix);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, Ref<Texture> texture)
	{
		Ref<MaterialInstance>& MI = s_Data->BasicTextureMaterialInstance;
		MI->SetTexture(MI->GetMaterial()->GetTextureSlots()[0], texture);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		Renderer::Draw(s_Data->QuadVertexArray, MI, transform, s_Data->VPMatrix);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, Ref<MaterialInstance> materialInstance)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		Renderer::Draw(s_Data->QuadVertexArray, materialInstance, transform, s_Data->VPMatrix);
	}

}