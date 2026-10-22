#include "Renderer2D.h"

#include "FoolsEngine/Scene/ECS.h"
#include "FoolsEngine/Scene/Scene.h"
#include "FoolsEngine/Scene/Component.h"
#include "FoolsEngine/Scene/Components/2DComponents.h"
#include "FoolsEngine/Scene/Components/RenderingComponents.h"
#include "FoolsEngine/Scene/GameplayWorld/Entity.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/1 - GAPI/Stream.h"
#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/2 - Representation/Camera.h"
#include "FoolsEngine/Renderer/2 - Representation/Texture.h"
#include "FoolsEngine/Renderer/2 - Representation/Material.h"
#include "FoolsEngine/Renderer/5 - Integration/Renderer.h"

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

namespace fe
{
	static Render::GAPI::GID sIndexBuffer;
	static Render::GAPI::GID sVertexDownStream;

	Renderer2D::RenderStats Renderer2D::m_Stats;

	static Renderer2D::BatchData m_Batch;
	static Time::TimePoint m_RenderStartTimePoint;
	static AssetHandle<Shader> m_BaseShader;

	void Renderer2D::Startup()
	{
		FE_PROFILER_FUNC();

		Pile p;

		{
			FE_PROFILER_SCOPE("QuadsIndexBufferData generation");

			sIndexBuffer = Render::GAPI::CreateBuffer();
			U32* indices = (U32*)Render::GAPI::AllocateBuffer(sIndexBuffer, ConstLimits::MaxIndices * sizeof(U32));

			for (UInt i = 0; i < ConstLimits::QuadsInBatch; i++)
			{
				indices[i * 6 + 0] = i * 4 + 0;
				indices[i * 6 + 1] = i * 4 + 1;
				indices[i * 6 + 2] = i * 4 + 2;
				
				indices[i * 6 + 3] = i * 4 + 2;
				indices[i * 6 + 4] = i * 4 + 3;
				indices[i * 6 + 5] = i * 4 + 0;
			}

			Render::GAPI::CommitBuffer(sIndexBuffer);
		}


		Context::Log::Core.Error("Renderer2D startup not implemented"_FE);

		//
		//layout.Type = Description::Buffer::LayoutType::Vertex;
		//layout.Elements.emplace_back(Description::Data::Type::Float3, "a_Position");
		//layout.Elements.emplace_back(Description::Data::Type::Float4, "a_Color");
		//layout.Elements.emplace_back(Description::Data::Type::Float2, "a_TexCoord");
		//layout.Elements.emplace_back(Description::Data::Type::Float,  "a_TilingFactor");
		//layout.Elements.emplace_back(Description::Data::Type::UInt,   "a_TextureSampler");
		//layout.Elements.emplace_back(Description::Data::Type::UInt,   "a_EntityID");

		//layout.CalculateOffsetsAndStride();


		//s_Instance->m_VertexArray.reset(new Resource::RMeshBindings_OpenGL());
		//s_Instance->m_VertexArray->LayoutID = layoutID;
		//s_Instance->m_VertexArray->Create();
		//s_Instance->m_VertexArray->BindIndexData(*s_Instance->m_QuadIndexBuffer, 0, 0);
		//s_Instance->m_VertexArray->BindVertexData(*s_Instance->m_QuadVertexBuffer, 0);

		//s_Instance.m_BaseShader = Renderer::BaseAssets.Shaders.Base2D;
		// moved to Renderer::AcquireBaseAssets()
		// to do: fix this bad architecture

		//s_Instance->m_BaseShaderTextureSlot = ShaderTextureSlot("u_Texture", Description::Texture::Type::Texture2D, 32);
		//for (unsigned int i = 0; i < ConstLimits::RendererTextureSlotsCount; i++)
		//	s_Instance->m_BaseShaderSamplers[i] = i;

		//s_Instance.m_Batch.Textures[0] = Renderer::BaseAssets.Textures.FlatWhite.GetID();
		// moved to Renderer::AcquireBaseAssets()
		// to do: fix this bad architecture
	}

	void Renderer2D::Shutdown()
	{
		FE_PROFILER_FUNC();
		
		FE_CORE_ASSERT(false, "Not implemented");
	}

	void Renderer2D::BeginScene()
	{
		FE_PROFILER_FUNC();


		FE_CORE_ASSERT(false, "Not implemented");
		//Command::PipelineState::SetDepthTest<GAPIType::OpenGL>(true);


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

	void Renderer2D::RenderScene(const AssetObserver& scene)
	{
		FE_PROFILER_FUNC();

		Context::Log::Core.Error("Renderer2::RenderScene not implemented"_FE);
		return;

		BeginScene();

		auto& registry = scene.Get<Scene::Core>().GameplayWorld->m_Registry;

		auto view_tiles = registry.view<CTile, CTransformGlobal>();

		for (auto ID : view_tiles)
		{
			auto [tile_comp, entity_transform_comp] = view_tiles.get(ID);
			if (!tile_comp.Tile.Texture.IsValid()) continue;
			if (tile_comp.Tile.Texture.GetLoadingPriority() == AssetLoadingPriority::None) continue;
			Transform transform = entity_transform_comp.GetRef() + tile_comp.Offset;
			BatchQuadDrawCall(tile_comp.Tile, transform, ID);
		}

		Flush();

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
			BatchQuadDrawCall(sprite_comp.Sprite, transform, ID);
			Flush();
		}

		EndScene();
	}

	void Renderer2D::EndScene()
	{
		FE_PROFILER_FUNC();

		m_Stats.RenderTime = Time::Now() - m_RenderStartTimePoint;
	}

	void Renderer2D::BatchQuadDrawCall(const Quad& quad, const Transform& transform, EntityID ID)
	{
		auto& VIt = m_Batch.QuadVeriticesIt;

		// nasty legacy frankenshtine
		if (&*VIt > &m_Batch.QuadVertices->operator[](m_Batch.QuadVertices->Count-1))
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
			auto& spec = texture_observer.Get<Texture2D::Core>().mSpecification;
			aspect_ratio = (float)spec.mDimentions.x / (float)spec.mDimentions.y;
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
		uint32_t data_size = (uint32_t)((uint8_t*)m_Batch.QuadVeriticesIt - (uint8_t*)m_Batch.QuadVertices.get());

		FE_CORE_ASSERT(false, "not implemented");
		//m_QuadVertexBuffer->Update(data_size, m_Batch.QuadVertices->data());

		for (unsigned int i = 0; i < m_Batch.TexturesCount; i++)
		{
			auto user = AssetUser(m_Batch.Textures[i]);
			FE_CORE_ASSERT(false, "not implemented");
			//auto texture_resource = user.GetResource<GAPIType::OpenGL>().Texture;
			//Command::PipelineState::BindTextureToRendererTextureSlot<GAPIType::OpenGL>(i, texture_resource);
		}

		FE_CORE_ASSERT(false, "not implemented");
		//m_VertexArray->IndexCount = m_Batch.QuadIndexCount;
		//Command::PipelineState::BindMeshBindings_OpenGL(*(Resource::RMeshBindings_OpenGL*)&*m_VertexArray);
		//
		//Command::Render::DrawIndexed(*(Resource::RMeshBindings_OpenGL*)&*m_VertexArray);

		m_Stats.Quads += m_Batch.QuadIndexCount / 3 / 2;
		m_Stats.DrawCalls++;

		ClearBatch();
	}

}