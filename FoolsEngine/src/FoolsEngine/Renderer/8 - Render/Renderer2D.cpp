#include "FE_pch.h"
#include "Renderer2D.h"

#include "FoolsEngine\Renderer\1 - Primitives\VertexData.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Framebuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\IndexBuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\3 - Representation\Camera.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Scene\ECS.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "FoolsEngine\Scene\Scene.h"

#include "FoolsEngine\Scene\Component.h"
#include "FoolsEngine\Scene\Components\RenderingComponents.h"
#include "FoolsEngine\Scene\Components\2DComponents.h"

#include <glad\glad.h>

#include <glm\gtc\type_ptr.hpp>

namespace fe
{
	Renderer2D::Renderer2DData& Renderer2D::s_Data = *(new Renderer2DData());
	Renderer2D::RenderStats Renderer2D::s_Stats;
	Time::TimePoint Renderer2D::m_RenderStartTimePoint;

	void Renderer2D::Init()
	{
		FE_PROFILER_FUNC();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(ConstLimits::QuadsInBatch * 4 * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderData::Type::Float3, "a_Position" },
			{ ShaderData::Type::Float4, "a_Color" },
			{ ShaderData::Type::Float2, "a_TexCoord" },
			{ ShaderData::Type::Float,  "a_TilingFactor" },
			{ ShaderData::Type::UInt,   "a_TextureSampler" },
			{ ShaderData::Type::UInt,   "a_EntityID" }
		});

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
		s_Data.QuadVertexBuffer->SetIndexBuffer(quadIB);
		delete quadIndices;

		s_Data.BaseShader = Renderer::BaseAssets.Shaders.Base2D;

		s_Data.BaseShaderTextureSlot = ShaderTextureSlot("u_Texture", TextureData::Type::Texture2D, 32);
		for (unsigned int i = 0; i < ConstLimits::RendererTextureSlotsCount; i++)
			s_Data.BaseShaderSamplers[i] = i;

		s_Data.Batch.Textures[0] = Renderer::BaseAssets.Textures.FlatWhite;
	}

	void Renderer2D::Shutdown()
	{
		delete &s_Data;
	}

	void Renderer2D::BeginScene(const glm::mat4& projection, const glm::mat4& view, Framebuffer& framebuffer)
	{
		FE_PROFILER_FUNC();

		framebuffer.Bind();

		RenderCommands::SetDepthTest(true);

		auto GDI = Renderer::GetActiveGDItype();

		switch (GDI)
		{
		case GDIType::OpenGL:
			s_Data.VPMatrix = projection * glm::inverse(view);
			break;
		default:
			FE_CORE_ASSERT(false, "Unkown GDI!");
			return;
		}

		ClearBatch();

		s_Stats.Quads = 0;
		s_Stats.DrawCalls = 0;
		m_RenderStartTimePoint = Time::Now();

		auto baseShader = s_Data.BaseShader.Use();

		baseShader.Bind(GDI);
		baseShader.UploadUniform(
			GDI,
			Uniform("u_ViewProjection", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(s_Data.VPMatrix)
		);
		baseShader.BindTextureSlot(
			GDI,
			s_Data.BaseShaderTextureSlot,
			s_Data.BaseShaderSamplers,
			ConstLimits::RendererTextureSlotsCount
		);
	}

	void Renderer2D::ClearBatch()
	{
		s_Data.Batch.TexturesCount = 1;
		s_Data.Batch.QuadIndexCount = 0;
		s_Data.Batch.QuadVeriticesIt = s_Data.Batch.QuadVertices->begin();
	}

	void Renderer2D::RenderScene(const AssetObserver<Scene>& scene, const Camera& camera, const Transform& cameraTransform, Framebuffer& framebuffer)
	{
		FE_PROFILER_FUNC();

		BeginScene(camera, cameraTransform, framebuffer);

		auto& registry = scene.GetDataComponent().GameplayWorld->GetRegistry();

		auto viewTiles = registry.view<CTile, CTransformGlobal>();

		bool noTiles = true;

		for (auto ID : viewTiles)
		{
			noTiles = false;
			auto [tile, entityTransform] = viewTiles.get(ID);
			Transform transform = entityTransform.GetRef() + tile.Offset;
			BatchQuadDrawCall(tile.Tile, transform, ID);
		}

		Flush();

		if (camera.GetProjectionType() == Camera::ProjectionType::Orthographic && cameraTransform.Rotation.x == 0 && cameraTransform.Rotation.y == 0)
			registry.sort<CSprite>([&](const EntityID l, const EntityID r) {
				auto& lz = registry.get<CTransformGlobal>(l).GetRef().Shift.z;
				auto& rz = registry.get<CTransformGlobal>(r).GetRef().Shift.z;

				return lz < rz;
			});
		else
			registry.sort<CSprite>([&](const EntityID l, const EntityID r) {
				const auto& lPosition = registry.get<CTransformGlobal>(l).GetRef().Shift;
				const auto& rPosition = registry.get<CTransformGlobal>(r).GetRef().Shift;

				auto lDistance = glm::distance(cameraTransform.Shift, lPosition);
				auto rDistance = glm::distance(cameraTransform.Shift, rPosition);

				return lDistance > rDistance;
			});

		auto viewSprites = registry.view<CSprite, CTransformGlobal>();

		for (auto ID : viewSprites)
		{
			if (noTiles)
			{
				int dummy = 0;
			}
			auto [sprite, entityTransform] = viewSprites.get(ID);
			Transform transform = entityTransform.GetRef() + sprite.Offset;
			BatchQuadDrawCall(sprite.Sprite, transform, ID);
			Flush();
		}

		EndScene(framebuffer);
	}

	void Renderer2D::EndScene(Framebuffer& framebuffer)
	{
		FE_PROFILER_FUNC();

		s_Stats.RenderTime = Time::Now() - m_RenderStartTimePoint;
	}

	void Renderer2D::BatchQuadDrawCall(const Quad& quad, const Transform& transform, EntityID ID)
	{
		auto& batch = s_Data.Batch;

		auto& VIt = batch.QuadVeriticesIt;

		if (&*VIt > &batch.QuadVertices->back())
		{
			Flush();
		}

		uint32_t textureSampler = 0;

		if (batch.Textures[0].GetID() != quad.Texture.GetID())
		{
			for (unsigned int i = 1; i < batch.TexturesCount; i++)
			{
				if (batch.Textures[i].GetID() == quad.Texture.GetID())
				{
					textureSampler = i;
					break;
				}
			}

			if (textureSampler == 0)
			{
				if (batch.TexturesCount >= ConstLimits::RendererTextureSlotsCount)
				{
					FE_CORE_ASSERT(false, "Renderer2D textures slots count exceeded!");
				}
				else
				{
					batch.Textures[batch.TexturesCount] = quad.Texture;
					textureSampler = batch.TexturesCount;
					batch.TexturesCount++;
				}
			}
		}

		float aspectRatio;
		{
			auto texture_observer = quad.Texture.Observe();
			auto& spec = texture_observer.GetDataComponent().Specification;
			aspectRatio = (float)spec.Height / (float)spec.Width;
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

		glm::mat4 transformMatrix = transform.GetMatrix();

		for (int i = 0; i < 4; i++)
		{
			glm::vec4 vertexPosition = QuadVertexPositions[i];
			vertexPosition.y *= aspectRatio;
			VIt->Shift = transformMatrix * vertexPosition;
			VIt->Color = quad.Color;
			VIt->TexCoord = TextureCoord[i];
			VIt->TilingFactor = quad.TextureTilingFactor;
			VIt->TextureSampler = textureSampler;
			VIt->EntityID = ID;
			VIt++;
		}

		batch.QuadIndexCount += 6;
	}

	void Renderer2D::Flush()
	{
		FE_PROFILER_FUNC();

		auto& batch = s_Data.Batch;

		if (batch.QuadIndexCount == 0)
			return;

		// propably C-style array would look cleaner here then std::array
		uint32_t dataSize = (uint32_t)((uint8_t*)batch.QuadVeriticesIt._Unwrapped() - (uint8_t*)batch.QuadVertices.get());

		s_Data.QuadVertexBuffer->Bind();
		s_Data.QuadVertexBuffer->SendDataToGPU(batch.QuadVertices->data(), dataSize);

		auto GDI = Renderer::GetActiveGDItype();

		for (unsigned int i = 0; i < batch.TexturesCount; i++)
		{
			batch.Textures[i].Use().Bind(GDI, i);
		}

		s_Data.QuadVertexBuffer->Bind();
		RenderCommands::DrawIndexed(s_Data.QuadVertexBuffer.get(), batch.QuadIndexCount);

		s_Stats.Quads += batch.QuadIndexCount / 3 / 2;
		s_Stats.DrawCalls++;

		ClearBatch();
	}

}