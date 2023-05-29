#include "FE_pch.h"
#include "Renderer2D.h"
#include "Renderer.h"

#include <glad\glad.h>

namespace fe
{
	Scope<Renderer2D::Renderer2DData> Renderer2D::s_Data = nullptr;
	Renderer2D::RenderStats Renderer2D::s_Stats;
	Time::TimePoint Renderer2D::m_RenderStartTimePoint;

	void Renderer2D::Init()
	{
		s_Data = CreateScope<Renderer2DData>();

		s_Data->QuadVertexArray = VertexArray::Create();

		s_Data->QuadVertexBuffer = VertexBuffer::Create(ConstLimits::QuadsInBatch * 4 * sizeof(QuadVertex));
		s_Data->QuadVertexBuffer->SetLayout({
			{ ShaderData::Type::Float3, "a_Position" },
			{ ShaderData::Type::Float4, "a_Color" },
			{ ShaderData::Type::Float2, "a_TexCoord" },
			{ ShaderData::Type::Float,  "a_TilingFactor" },
			{ ShaderData::Type::UInt,   "a_TextureSampler" }
		});
		s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);
		
		using QuadsIndexBufferData = std::array<uint32_t, ConstLimits::MaxIndices>;
		QuadsIndexBufferData* quadIndices = new QuadsIndexBufferData();
		uint32_t offset = 0;
		for(auto indicesPtr = quadIndices->begin(); indicesPtr != quadIndices->end(); )
		{
			*(indicesPtr++) = offset + 0;
			*(indicesPtr++) = offset + 1;
			*(indicesPtr++) = offset + 2;

			*(indicesPtr++) = offset + 2;
			*(indicesPtr++) = offset + 3;
			*(indicesPtr++) = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices->data(), ConstLimits::MaxIndices);
		s_Data->QuadVertexArray->SetIndexBuffer(quadIB);
		delete quadIndices;

		s_Data->BaseShader = Shader::Create("assets/shaders/Base2DShader.glsl");
		s_Data->BaseShaderTextureSlot = ShaderTextureSlot("u_Texture", TextureType::Texture2D, 32);
		for (int i = 0; i < ConstLimits::RendererTextureSlotsCount; i++)
			s_Data->BaseShaderSamplers[i] = i;

		Ref<Texture> whiteTexture = Texture2D::Create("Base2DTexture", 1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		TextureLibrary::Add(whiteTexture);

		s_Data->OpaqueBatch.Textures[0] = whiteTexture;
		for (auto& batch : s_Data->TransparentBatches)
		batch.Textures[0] = whiteTexture;
	}

	void Renderer2D::BeginScene(OrthographicCamera& camera)
	{
		FE_PROFILER_FUNC();

		RenderCommands::Clear();
		RenderCommands::SetClearColor({ 0.1, 0.1, 0.1, 1 });

		s_Data->VPMatrix = camera.GetViewProjectionMatrix();

		s_Data->OpaqueBatch.TexturesCount = 1;
		s_Data->OpaqueBatch.QuadIndexCount = 0;
		s_Data->OpaqueBatch.QuadVeriticesIt = s_Data->OpaqueBatch.QuadVertices->begin();

		for (auto& batch : s_Data->TransparentBatches)
		{
			batch.TexturesCount = 1;
			batch.QuadIndexCount = 0;
			batch.QuadVeriticesIt = batch.QuadVertices->begin();
		}
	}

	void Renderer2D::EndScene()
	{
		FE_PROFILER_FUNC();

		s_Stats.Quads = 0;
		s_Stats.DrawCalls = 0;
		m_RenderStartTimePoint = Time::Now();

		s_Data->BaseShader->Bind();
		s_Data->BaseShader->UploadUniform(
			Uniform("u_ViewProjection", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(s_Data->VPMatrix)
		);
		s_Data->BaseShader->BindTextureSlot(s_Data->BaseShaderTextureSlot, s_Data->BaseShaderSamplers, ConstLimits::RendererTextureSlotsCount);

		Flush(s_Data->OpaqueBatch, false);
		for (int i = 0; i < 19; i++)
		{
			Flush(s_Data->TransparentBatches[i], true);
		}

		s_Stats.RenderTime = Time::Now() - m_RenderStartTimePoint;
	}

	void Renderer2D::DrawQuad(const Quad& quad)
	{
		BatchData* batch = &(s_Data->OpaqueBatch);

		if (quad.Transparency)
		{
			batch = &(s_Data->TransparentBatches[quad.Layer+9]);
		}

		BatchQuadDrawCall(quad, *batch);
	}

	void Renderer2D::BatchQuadDrawCall(const Quad& quad, BatchData& batch)
	{
		auto& VIt = batch.QuadVeriticesIt;

		FE_CORE_ASSERT(VIt < batch.QuadVertices->end(), "Renderer2D batch capacity exceeded!");

		uint32_t textureSampler = 0;

		if (batch.Textures[0].get() != quad.Texture.get())
		{
			for (unsigned int i = 1; i < batch.TexturesCount; i++)
			{
				if (batch.Textures[i].get() == quad.Texture.get())
				{
					textureSampler = i;
					break;
				}
			}

			if (textureSampler == 0)
			{
				FE_CORE_ASSERT(batch.TexturesCount < ConstLimits::RendererTextureSlotsCount, "Renderer2D textures slots count exceeded!");

				batch.Textures[batch.TexturesCount] = quad.Texture;
				textureSampler = batch.TexturesCount;
				batch.TexturesCount++;
			}

		}

		constexpr glm::vec4 QuadVertexPositions[] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};
		constexpr glm::vec2 TextureCoord[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		float depth = (float)quad.Layer / 10.0f;

		glm::mat4 transform =
			  glm::translate(glm::mat4(1.0f), { quad.Position, depth })
			* glm::rotate   (glm::mat4(1.0f), glm::radians(quad.Rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale    (glm::mat4(1.0f), { quad.Size.x, quad.Size.y, 1.0f });

		for (int i = 0; i < 4; i++)
		{
			VIt->Position = transform * QuadVertexPositions[i];
			VIt->Color = quad.Color;
			VIt->TexCoord = TextureCoord[i];
			VIt->TilingFactor = quad.TextureTilingFactor;
			VIt->TextureSampler = textureSampler;
			VIt++;
		}

		batch.QuadIndexCount += 6;
	}

	void Renderer2D::Flush(BatchData& batch, bool transparency)
	{
		if (batch.QuadIndexCount == 0)
			return;

		// propably C-style array would look cleaner here then std::array
		uint32_t dataSize = (uint32_t)((uint8_t*)batch.QuadVeriticesIt._Unwrapped() - (uint8_t*)batch.QuadVertices.get());

		s_Data->QuadVertexBuffer->Bind();
		s_Data->QuadVertexBuffer->SetData(batch.QuadVertices->data(), dataSize);

		for (unsigned int i = 0; i < batch.TexturesCount; i++)
		{
			batch.Textures[i]->Bind(i);
		}

		s_Data->QuadVertexArray->Bind();
		RenderCommands::DrawIndexed(s_Data->QuadVertexArray, batch.QuadIndexCount);

		s_Stats.Quads += batch.QuadIndexCount / 3 / 2;
		s_Stats.DrawCalls++;
	}

 }