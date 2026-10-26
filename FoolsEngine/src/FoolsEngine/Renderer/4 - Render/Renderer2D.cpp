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
#include "FoolsEngine/Renderer/2 - Representation/Framebuffer.h"
#include "FoolsEngine/Renderer/5 - Integration/Renderer.h"

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

namespace fe
{
	static Render::GAPI::GID sIndexBuffer;

	Renderer2D::RenderStats Renderer2D::m_Stats;

	static Renderer2D::BatchData m_Batch;
	static Time::TimePoint m_RenderStartTimePoint;

	static Render::GAPI::GID sQuadsStream;

	static Render::GAPI::GID sPipeline;

	struct alignas(16) QuadGPU
	{
		Render::GAPI::std140_mat4x4 Transform;
		Render::GAPI::std140_vec4 Color;
		Render::GAPI::std140_vec2 AspecRatio;
		Render::GAPI::std140_vec2 TextureView;
		Render::GAPI::std140_float TextureTilingFactor = 1.0f;
		Render::GAPI::std140_float Padding = 1.0f;
		Render::GAPI::std140_float Padding2 = 1.0f;
		Render::GAPI::std140_float Padding3 = 1.0f;
	};

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

		{
			FE_PROFILER_SCOPE("QuadsStream creation");

			sQuadsStream = Render::GAPI::CreateDownStream();
			Render::GAPI::AllocateDownStream(sQuadsStream, ConstLimits::QuadsInBatch * 2);
		}
	}

	void Renderer2D::AcquireBaseAssets()
	{
		FE_PROFILER_FUNC();

		{
			FE_PROFILER_SCOPE("Pipeline creation");

			Render::GAPI::GID vertex_id = Renderer::BaseAssets.Shaders.Base2DVertex.Observe().Get<ACShader_GID<Render::GAPI::Platform::OpenGL>>().mShaderGID;
			Render::GAPI::GID fragment_id = Renderer::BaseAssets.Shaders.Base2DFragment.Observe().Get<ACShader_GID<Render::GAPI::Platform::OpenGL>>().mShaderGID;

			Render::GAPI::Raster::Specification specification
			{
				.mPrimitiveType = Render::GAPI::Raster::PrimitiveType::Triangle,
				.mFaceCullTest = Render::GAPI::Raster::FaceCullTest::Always,
				.mDepthStencilFormat = Render::GAPI::Descriptors::TextureFormat::DEPTH24STENCIL8
			};

			specification.mColorAttachments.Append(Render::GAPI::Raster::Attachment
			{
				.mFormat = Render::GAPI::Descriptors::TextureFormat::RGB_8
			});

			specification.mColorAttachments.Append(Render::GAPI::Raster::Attachment
			{
				.mFormat = Render::GAPI::Descriptors::TextureFormat::R_UINT_32
			});

			sPipeline = Render::GAPI::CreateGraphicsPipelineCmd(vertex_id, fragment_id, specification);
			
			Render::GAPI::SetDepthStencilSpec(sPipeline, Render::GAPI::DepthStencil::Specification {
				.mDepthTestType = Render::GAPI::DepthStencil::DepthTestType::Less,
				.mStencilTestType = Render::GAPI::DepthStencil::StencilTestType::Always,
				.mStencilCases = { },
				.mDepthTest = true
			});
			
			Render::GAPI::SetBlendSpec(sPipeline, Render::GAPI::Blend::Specification{
				.mBlendFunction = Render::GAPI::Blend::BlendFunction::None
			});
			
			Render::GAPI::SetUBO(sPipeline, sQuadsStream, 1);
			Render::GAPI::SetIndexSource(sPipeline, sIndexBuffer);

			//SetColorAttachment(GID pipeline, GID texture, UInt index);
			//SetDepthStencilAttachment(GID pipeline, GID texture);
		}
	}

	void Renderer2D::Shutdown()
	{
		FE_PROFILER_FUNC();
		
		
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

		//auto base_shader = m_BaseShader.Use();

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
		FE_CORE_ASSERT(false, "Not implemented");
		//m_Batch.TexturesCount = 1;
		//m_Batch.QuadIndexCount = 0;
		//m_Batch.QuadVeriticesIt = m_Batch.QuadVertices->begin();
	}

	void Renderer2D::RenderScene(const AssetObserver& scene, const Render::Representation::Framebuffer& framebuffer)
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
		FE_CORE_ASSERT(false, "Not implemented");

		//auto& VIt = m_Batch.QuadVeriticesIt;

		// nasty legacy frankenshtine
		//if (&*VIt > &m_Batch.QuadVertices->operator[](m_Batch.QuadVertices->Count-1))
		//{
		//	Flush();
		//}

		uint32_t texture_sampler_index = 0;

		//if (m_Batch.Textures[0] != quad.Texture.GetID())
		//{
		//	for (unsigned int i = 1; i < m_Batch.TexturesCount; i++)
		//	{
		//		if (m_Batch.Textures[i] == quad.Texture.GetID())
		//		{
		//			texture_sampler_index = i;
		//			break;
		//		}
		//	}
		//
		//	if (texture_sampler_index == 0)
		//	{
		//		if (m_Batch.TexturesCount >= ConstLimits::RendererTextureSlotsCount)
		//		{
		//			FE_CORE_ASSERT(false, "Renderer2D textures slots count exceeded!");
		//		}
		//		else
		//		{
		//			m_Batch.Textures[m_Batch.TexturesCount] = quad.Texture.GetID();
		//			texture_sampler_index = m_Batch.TexturesCount;
		//			m_Batch.TexturesCount++;
		//		}
		//	}
		//}

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

		//for (int i = 0; i < 4; i++)
		//{
		//	glm::vec4 vertex_position = quad_vertex_positions[i];
		//	vertex_position.y *= aspect_ratio;
		//	VIt->Shift = transform_matrix * vertex_position;
		//	VIt->Color = quad.Color;
		//	VIt->TexCoord = texture_coords[i];
		//	VIt->TilingFactor = quad.TextureTilingFactor;
		//	VIt->TextureSampler = texture_sampler_index;
		//	VIt->EntityID = ID;
		//	VIt++;
		//}

		m_Batch.QuadIndexCount += 6;
	}

	void Renderer2D::Flush()
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(false, "Not implemented");

		if (m_Batch.QuadIndexCount == 0)
			return;

		// propably C-style array would look cleaner here then std::array
		//uint32_t data_size = (uint32_t)((uint8_t*)m_Batch.QuadVeriticesIt - (uint8_t*)m_Batch.QuadVertices.get());

		FE_CORE_ASSERT(false, "not implemented");
		//m_QuadVertexBuffer->Update(data_size, m_Batch.QuadVertices->data());

		//for (unsigned int i = 0; i < m_Batch.TexturesCount; i++)
		//{
		//	auto user = AssetUser(m_Batch.Textures[i]);
		//	FE_CORE_ASSERT(false, "not implemented");
		//	//auto texture_resource = user.GetResource<GAPIType::OpenGL>().Texture;
		//	//Command::PipelineState::BindTextureToRendererTextureSlot<GAPIType::OpenGL>(i, texture_resource);
		//}

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