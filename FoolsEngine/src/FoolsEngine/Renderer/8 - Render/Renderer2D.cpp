#include "FE_pch.h"
#include "Renderer2D.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"

#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
//#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\HierarchyDirector.h"
#include "FoolsEngine\Scene\Scene.h"
#include "FoolsEngine\Scene\Component.h"

#include <glad\glad.h>

namespace fe
{
	Scope<Renderer2D::Renderer2DData> Renderer2D::s_Data = nullptr;
	Renderer2D::RenderStats Renderer2D::s_Stats;
	Time::TimePoint Renderer2D::m_RenderStartTimePoint;

	void Renderer2D::Init()
	{
		FE_PROFILER_FUNC();

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
		for (auto indicesPtr = quadIndices->begin(); indicesPtr != quadIndices->end(); )
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
		s_Data->BaseShaderTextureSlot = ShaderTextureSlot("u_Texture", TextureData::Type::Texture2D, 32);
		for (int i = 0; i < ConstLimits::RendererTextureSlotsCount; i++)
			s_Data->BaseShaderSamplers[i] = i;

		Ref<Texture> whiteTexture = Texture2D::Create("Base2DTexture", 1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		TextureLibrary::Add(whiteTexture);

		s_Data->Batch.Textures[0] = whiteTexture;
	}

	void Renderer2D::BeginScene(const glm::mat4& projection, const glm::mat4& view)
	{
		FE_PROFILER_FUNC();

		RenderCommands::SetDepthTest(true);
		RenderCommands::Clear();
		RenderCommands::SetClearColor({ 0.1, 0.1, 0.1, 1 });

		switch (Renderer::GetActiveGDItype())
		{
		case GDIType::OpenGL:
			s_Data->VPMatrix = projection * glm::inverse(view);
			break;
		default:
			FE_CORE_ASSERT(false, "Unkown GDI!");
			return;
		}

		ClearBatch();

		s_Stats.Quads = 0;
		s_Stats.DrawCalls = 0;
		m_RenderStartTimePoint = Time::Now();

		s_Data->BaseShader->Bind();
		s_Data->BaseShader->UploadUniform(
			Uniform("u_ViewProjection", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(s_Data->VPMatrix)
		);
		s_Data->BaseShader->BindTextureSlot(s_Data->BaseShaderTextureSlot, s_Data->BaseShaderSamplers, ConstLimits::RendererTextureSlotsCount);
	}

	void Renderer2D::ClearBatch()
	{
		s_Data->Batch.TexturesCount = 1;
		s_Data->Batch.QuadIndexCount = 0;
		s_Data->Batch.QuadVeriticesIt = s_Data->Batch.QuadVertices->begin();
	}

	void Renderer2D::RenderScene(Scene& scene, Entity cameraSet)
	{
		auto& projectionMatrix = scene.GetGameplayWorld()->GetRegistry().get<CCamera>(cameraSet).Camera;
		auto& cameraTransform = scene.GetGameplayWorld()->GetRegistry().get<CTransformGlobal>(cameraSet).GetRef();
		RenderScene(scene, projectionMatrix, cameraTransform);
	}

	void Renderer2D::RenderScene(Scene& scene, const Camera& camera, const Transform& cameraTransform)
	{
		FE_PROFILER_FUNC();

		BeginScene(camera, cameraTransform);

		auto& registry = scene.GetGameplayWorld()->GetRegistry();

		auto& viewTiles = registry.view<CTile, CTransformGlobal>();

		for (auto ID : viewTiles)
		{
			auto [tile, transform] = viewTiles.get(ID);
			BatchQuadDrawCall(tile.Tile, transform.GetRef());
		}

		Flush();

		if (camera.GetProjectionType() == Camera::ProjectionType::Orthographic && cameraTransform.Rotation.x == 0 && cameraTransform.Rotation.y == 0)
			registry.sort<CSprite>([&](const EntityID l, const EntityID r) {
				auto& lz = registry.get<CTransformGlobal>(l).GetRef().Position.z;
				auto& rz = registry.get<CTransformGlobal>(r).GetRef().Position.z;

				return lz < rz;
			});
		else
			registry.sort<CSprite>([&](const EntityID l, const EntityID r) {
				const auto& lPosition = registry.get<CTransformGlobal>(l).GetRef().Position;
				const auto& rPosition = registry.get<CTransformGlobal>(r).GetRef().Position;

				auto lDistance = glm::distance(cameraTransform.Position, lPosition);
				auto rDistance = glm::distance(cameraTransform.Position, rPosition);

				return lDistance > rDistance;
			});

		auto& viewSprites = registry.view<CSprite, CTransformGlobal>();

		for (auto ID : viewSprites)
		{
			auto [sprite, transform] = viewSprites.get(ID);
			BatchQuadDrawCall(sprite.Sprite, transform.GetRef());
			Flush();
		}

		EndScene();
	}

	void Renderer2D::EndScene()
	{
		FE_PROFILER_FUNC();

		s_Stats.RenderTime = Time::Now() - m_RenderStartTimePoint;
	}

	void Renderer2D::BatchQuadDrawCall(const Quad& quad, const Transform& transform)
	{
		auto& batch = s_Data->Batch;

		auto& VIt = batch.QuadVeriticesIt;

		if (&*VIt > &batch.QuadVertices->back())
		{
			Flush();
		}

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

		glm::mat4 transformMatrix = transform.GetTransform();

		for (int i = 0; i < 4; i++)
		{
			VIt->Position = transformMatrix * QuadVertexPositions[i];
			VIt->Color = quad.Color;
			VIt->TexCoord = TextureCoord[i];
			VIt->TilingFactor = quad.TextureTilingFactor;
			VIt->TextureSampler = textureSampler;
			VIt++;
		}

		batch.QuadIndexCount += 6;
	}

	void Renderer2D::Flush()
	{
		FE_PROFILER_FUNC();

		auto& batch = s_Data->Batch;

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

		ClearBatch();
	}

}