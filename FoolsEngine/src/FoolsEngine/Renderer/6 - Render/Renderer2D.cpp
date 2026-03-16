#include "FE_pch.h"
#include "Renderer2D.h"

#include "FoolsEngine/Scene/ECS.h"
#include "FoolsEngine/Scene/Scene.h"
#include "FoolsEngine/Scene/Component.h"
#include "FoolsEngine/Scene/Components/2DComponents.h"
#include "FoolsEngine/Scene/Components/RenderingComponents.h"
#include "FoolsEngine/Scene/GameplayWorld/Entity.h"

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"
#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/2 - Resource/Framebuffer.h"
#include "FoolsEngine/Renderer/3 - Command/Render.h"
#include "FoolsEngine/Renderer/3 - Command/PipelineState.h"
#include "FoolsEngine/Renderer/3 - Command/ResourceState.h"
#include "FoolsEngine/Renderer/4 - Representation/Camera.h"
#include "FoolsEngine/Renderer/4 - Representation/Texture.h"
#include "FoolsEngine/Renderer/4 - Representation/Material.h"
#include "FoolsEngine/Renderer/7 - Integration/Renderer.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace fe
{
	Renderer2D* Renderer2D::s_Instance;

	void Renderer2D::Startup()
	{
		s_Instance = new Renderer2D;
	}

	void Renderer2D::Init()
	{
		FE_PROFILER_FUNC();

		Scratchpad sp;

		s_Instance->m_QuadVertexBuffer.reset(new Resource::StaticBuffer_OpenGL());
		s_Instance->m_QuadVertexBuffer->Usage = Description::Buffer::Usage::Vertex;
		s_Instance->m_QuadVertexBuffer->Size = ConstLimits::QuadsInBatch * 4 * sizeof(QuadVertex);
		s_Instance->m_QuadVertexBuffer->Create();

		uint32_t layoutID = (uint32_t)Description::Library::Get().BufferLayouts.size();
		auto& layout = Description::Library::Get().BufferLayouts.emplace_back();

		layout.Type = Description::Buffer::LayoutType::Vertex;
		layout.Elements.emplace_back(Description::Data::Type::Float3, "a_Position");
		layout.Elements.emplace_back(Description::Data::Type::Float4, "a_Color");
		layout.Elements.emplace_back(Description::Data::Type::Float2, "a_TexCoord");
		layout.Elements.emplace_back(Description::Data::Type::Float,  "a_TilingFactor");
		layout.Elements.emplace_back(Description::Data::Type::UInt,   "a_TextureSampler");
		layout.Elements.emplace_back(Description::Data::Type::UInt,   "a_EntityID");

		layout.CalculateOffsetsAndStride();

		using QuadsIndexBufferData = std::array<uint32_t, ConstLimits::MaxIndices>;
		QuadsIndexBufferData* quad_indices = sp.NewObject<QuadsIndexBufferData>();

		{
			FE_PROFILER_SCOPE("QuadsIndexBufferData generation");
						
			size_t quad_count = quad_indices->size() / 6;
			for (size_t i = 0; i < quad_count; i++)
			{
				(*quad_indices)[i * 6 + 0] = (uint32_t)(i * 4 + 0);
				(*quad_indices)[i * 6 + 1] = (uint32_t)(i * 4 + 1);
				(*quad_indices)[i * 6 + 2] = (uint32_t)(i * 4 + 2);
			
				(*quad_indices)[i * 6 + 3] = (uint32_t)(i * 4 + 2);
				(*quad_indices)[i * 6 + 4] = (uint32_t)(i * 4 + 3);
				(*quad_indices)[i * 6 + 5] = (uint32_t)(i * 4 + 0);
			}
		}

		s_Instance->m_QuadIndexBuffer.reset(new Resource::StaticBuffer_OpenGL());
		s_Instance->m_QuadIndexBuffer->Usage = Description::Buffer::Usage::Index;
		s_Instance->m_QuadIndexBuffer->Size = ConstLimits::MaxIndices;
		s_Instance->m_QuadIndexBuffer->Create();
		s_Instance->m_QuadIndexBuffer->Upload(ConstLimits::MaxIndices, quad_indices->data());

		s_Instance->m_VertexArray.reset(new Resource::VertexArray_OpenGL());
		s_Instance->m_VertexArray->LayoutID = layoutID;
		s_Instance->m_VertexArray->Create();
		s_Instance->m_VertexArray->BindIndexBuffer(*s_Instance->m_QuadIndexBuffer, 0, 0);
		s_Instance->m_VertexArray->BindVertexBuffer(*s_Instance->m_QuadVertexBuffer, 0);

		//s_Instance.m_BaseShader = Renderer::BaseAssets.Shaders.Base2D;
		// moved to Renderer::AcquireBaseAssets()
		// to do: fix this bad architecture

		//s_Instance->m_BaseShaderTextureSlot = ShaderTextureSlot("u_Texture", Description::Texture::Type::Texture2D, 32);
		for (unsigned int i = 0; i < ConstLimits::RendererTextureSlotsCount; i++)
			s_Instance->m_BaseShaderSamplers[i] = i;

		//s_Instance.m_Batch.Textures[0] = Renderer::BaseAssets.Textures.FlatWhite.GetID();
		// moved to Renderer::AcquireBaseAssets()
		// to do: fix this bad architecture
	}

	void Renderer2D::Shutdown()
	{
		FE_PROFILER_FUNC();
		delete s_Instance;
	}

	void Renderer2D::BeginScene()
	{
		FE_PROFILER_FUNC();

		Command::PipelineState::SetDepthTest<GAPIType::OpenGL>(true);

		auto GAPI = Renderer::GetActiveGAPIType();

		ClearBatch();

		m_Stats.Quads = 0;
		m_Stats.DrawCalls = 0;
		m_RenderStartTimePoint = Time::Now();

		auto base_shader = m_BaseShader.Use();

		// this needs to use a shading model, not a shader

		//base_shader.Bind(GAPI);
		//base_shader.UploadUniform(
		//	GAPI,
		//	Uniform("u_ViewProjection", Description::Data::Type::Mat4),
		//	(void*)glm::value_ptr(Renderer::SceneData.VPMatrix)
		//);
		//base_shader.BindTextureSlot(
		//	GAPI,
		//	m_BaseShaderTextureSlot,
		//	m_BaseShaderSamplers,
		//	ConstLimits::RendererTextureSlotsCount
		//);
	}

	void Renderer2D::ClearBatch()
	{
		m_Batch.TexturesCount = 1;
		m_Batch.QuadIndexCount = 0;
		m_Batch.QuadVeriticesIt = m_Batch.QuadVertices->begin();
	}

	void Renderer2D::RenderScene(const AssetObserver<Scene>& scene)
	{
		FE_PROFILER_FUNC();

		s_Instance->BeginScene();

		auto& registry = scene.GetCoreComponent().GameplayWorld->m_Registry;

		auto view_tiles = registry.view<CTile, CTransformGlobal>();

		for (auto ID : view_tiles)
		{
			auto [tile_comp, entity_transform_comp] = view_tiles.get(ID);
			if (!tile_comp.Tile.Texture.IsValid()) continue;
			if (tile_comp.Tile.Texture.GetLoadingPriority() == AssetLoadingPriority::None) continue;
			Transform transform = entity_transform_comp.GetRef() + tile_comp.Offset;
			s_Instance->BatchQuadDrawCall(tile_comp.Tile, transform, ID);
		}

		s_Instance->Flush();

		registry.sort<CSprite>([&](const EntityID l, const EntityID r) {
			const auto& lPosition = registry.get<CTransformGlobal>(l).GetRef().Shift;
			const auto& rPosition = registry.get<CTransformGlobal>(r).GetRef().Shift;

			auto lDistance = glm::distance(Renderer::SceneData.CameraTransform.Shift, lPosition);
			auto rDistance = glm::distance(Renderer::SceneData.CameraTransform.Shift, rPosition);

			return lDistance > rDistance;
		});

		auto view_sprites = registry.view<CSprite, CTransformGlobal>();

		for (auto ID : view_sprites)
		{
			auto [sprite_comp, entity_transform_comp] = view_sprites.get(ID);
			if (!sprite_comp.Sprite.Texture.IsValid()) continue;
			if (sprite_comp.Sprite.Texture.GetLoadingPriority() == AssetLoadingPriority::None) continue;
			Transform transform = entity_transform_comp.GetRef() + sprite_comp.Offset;
			s_Instance->BatchQuadDrawCall(sprite_comp.Sprite, transform, ID);
			s_Instance->Flush();
		}

		s_Instance->EndScene();
	}

	void Renderer2D::EndScene()
	{
		FE_PROFILER_FUNC();

		m_Stats.RenderTime = Time::Now() - m_RenderStartTimePoint;
	}

	void Renderer2D::BatchQuadDrawCall(const Quad& quad, const Transform& transform, EntityID ID)
	{
		auto& VIt = m_Batch.QuadVeriticesIt;

		if (&*VIt > &m_Batch.QuadVertices->back())
		{
			Flush();
		}

		uint32_t texture_sampler_index = 0;

		if (m_Batch.Textures[0] != quad.Texture.GetID())
		{
			for (unsigned int i = 1; i < m_Batch.TexturesCount; i++)
			{
				if (m_Batch.Textures[i] == quad.Texture.GetID())
				{
					texture_sampler_index = i;
					break;
				}
			}

			if (texture_sampler_index == 0)
			{
				if (m_Batch.TexturesCount >= ConstLimits::RendererTextureSlotsCount)
				{
					FE_CORE_ASSERT(false, "Renderer2D textures slots count exceeded!");
				}
				else
				{
					m_Batch.Textures[m_Batch.TexturesCount] = quad.Texture.GetID();
					texture_sampler_index = m_Batch.TexturesCount;
					m_Batch.TexturesCount++;
				}
			}
		}

		float aspect_ratio;
		{
			auto texture_observer = quad.Texture.Observe();
			auto& spec = texture_observer.GetCoreComponent().Specification;
			aspect_ratio = (float)spec.Height / (float)spec.Width;
		}

		constexpr glm::vec4 quad_vertex_positions[] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};

		constexpr glm::vec2 texture_coords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		glm::mat4 transform_matrix = transform.GetMatrix();

		for (int i = 0; i < 4; i++)
		{
			glm::vec4 vertex_position = quad_vertex_positions[i];
			vertex_position.y *= aspect_ratio;
			VIt->Shift = transform_matrix * vertex_position;
			VIt->Color = quad.Color;
			VIt->TexCoord = texture_coords[i];
			VIt->TilingFactor = quad.TextureTilingFactor;
			VIt->TextureSampler = texture_sampler_index;
			VIt->EntityID = ID;
			VIt++;
		}

		m_Batch.QuadIndexCount += 6;
	}

	void Renderer2D::Flush()
	{
		FE_PROFILER_FUNC();

		if (m_Batch.QuadIndexCount == 0)
			return;

		// propably C-style array would look cleaner here then std::array
		uint32_t data_size = (uint32_t)((uint8_t*)m_Batch.QuadVeriticesIt._Unwrapped() - (uint8_t*)m_Batch.QuadVertices.get());

		m_QuadVertexBuffer->Upload(data_size, m_Batch.QuadVertices->data());

		auto GAPI = Renderer::GetActiveGAPIType();

		for (unsigned int i = 0; i < m_Batch.TexturesCount; i++)
		{
			auto user = AssetUser<Texture2D>(m_Batch.Textures[i]);
			auto texture_resource = user.GetResourceComponent<GAPIType::OpenGL>().Texture;
			Command::PipelineState::BindTextureToRendererTextureSlot<GAPIType::OpenGL>(i, texture_resource);
		}

		m_VertexArray->IndexCount = m_Batch.QuadIndexCount;
		Command::PipelineState::BindVertexArray<GAPIType::OpenGL>(*m_VertexArray);

		Command::Render::DrawIndexed<GAPIType::OpenGL>(*m_VertexArray);

		m_Stats.Quads += m_Batch.QuadIndexCount / 3 / 2;
		m_Stats.DrawCalls++;

		ClearBatch();
	}

}