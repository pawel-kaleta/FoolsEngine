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
	static Render::GAPI::GID sQuadsStream;
	static Render::GAPI::GID sDrawParamsStream; // make this common for the whole rendering?
	static Render::GAPI::GID sPipeline;

	Renderer2D::RenderStats Renderer2D::m_Stats;

	static Renderer2D::BatchData m_Batch;
	static Time::TimePoint m_RenderStartTimePoint;


	struct alignas(16) QuadGPU
	{
		Render::GAPI::std140_mat4x4	Transform;
		Render::GAPI::std140_vec4	Color;
		Render::GAPI::std140_uvec2	TextureView;
		Render::GAPI::std140_float	AspecRatio;
		Render::GAPI::std140_float	TextureTilingFactor = 1.0f;
		Render::GAPI::std140_uint	EntityID;
		Render::GAPI::std140_float	Padding = 1.0f;
		Render::GAPI::std140_float	Padding2 = 1.0f;
		Render::GAPI::std140_float	Padding3 = 1.0f;
		Render::GAPI::std140_float	Padding4 = 1.0f;
	};

	struct alignas(16) RootData
	{
		Render::GAPI::std140_uint SceneDataIndex;
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

		{
			FE_PROFILER_SCOPE("DrawParamsStream creation");

			sDrawParamsStream = Render::GAPI::CreateDownStream();
			constinit static const U32 stream_size = (sizeof(Render::GAPI::DrawParams) + sizeof(RootData)) * 4;
			Render::GAPI::AllocateDownStream(sQuadsStream, stream_size);
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
			
			Render::GAPI::SetUBO(sPipeline, sDrawParamsStream, 0); // root data
			Render::GAPI::SetUBO(sPipeline, Renderer::sSceneDataStream, 1);
			Render::GAPI::SetUBO(sPipeline, sQuadsStream, 2);
			Render::GAPI::SetIndexSource(sPipeline, sIndexBuffer);
			Render::GAPI::SetDrawParamsSource(sPipeline, sDrawParamsStream);
		}
	}

	void Renderer2D::Shutdown()
	{
		FE_PROFILER_FUNC();
		
		
	}

	void Renderer2D::BeginScene(const Render::Representation::Framebuffer& framebuffer)
	{
		FE_PROFILER_FUNC();

		Render::GAPI::SetColorAttachment(sPipeline, framebuffer.mColorAttachments[0].mGID, 0);
		Render::GAPI::SetColorAttachment(sPipeline, framebuffer.mColorAttachments[1].mGID, 1);
		Render::GAPI::SetDepthStencilAttachment(sPipeline, framebuffer.mDepthStencilAttachment.mGID);

		m_Stats.Quads = 0;
		m_Stats.DrawCalls = 0;
		m_RenderStartTimePoint = Time::Now();

		m_Batch.QuadIndexCount = 0;

		Render::GAPI::ActivatePipelineCmd(sPipeline);
	}

	void Renderer2D::RenderScene(const AssetObserver& scene, const Render::Representation::Framebuffer& framebuffer)
	{
		FE_PROFILER_FUNC();

		Context::Log::Core.Error("Renderer2::RenderScene not implemented"_FE);
		
		BeginScene(framebuffer);

		auto& registry = scene.Get<Scene::Core>().GameplayWorld->m_Registry;

		auto view_tiles = registry.view<CTile, CTransformGlobal>();

		UInt tiles_count = view_tiles.size_hint();
		constinit const static U32 quad_offset_align = Render::GAPI::GetOffsetAlignmentFor<QuadGPU>();
		 
		Render::GAPI::GID region = Render::GAPI::CreateRegion(sQuadsStream, tiles_count * sizeof(QuadGPU), quad_offset_align);
		QuadGPU* quad = (QuadGPU*) Render::GAPI::GetRegionLocation(region);

		for (auto ID : view_tiles)
		{
			auto [tile_comp, entity_transform_comp] = view_tiles.get(ID);

			auto& tile = tile_comp.Tile;

			if (!tile.Texture.IsValid()) continue;
			if (tile.Texture.GetLoadingPriority() == AssetLoadingPriority::None) continue;

			//to do: frustum culing

			Transform transform = entity_transform_comp.GetRef() + tile_comp.Offset;
			auto transform_mat = transform.GetMatrix();

			auto texture_observer = tile.Texture.Observe();
			auto texture_view_id = texture_observer.Get<ACTexture2D_GPU<Render::GAPI::Platform::OpenGL>>().mTextureViewsGIDs[0]; // 0 is dogy, put this on tile?
			auto& spec = texture_observer.Get<Texture2D::Core>().mSpecification;
			
			quad->EntityID = ID;
			quad->Transform = * (Render::GAPI::std140_mat4x4*) & transform_mat;
			quad->Color = { tile.Color };
			quad->TextureView = Render::GAPI::GetTextureViewHandle(texture_view_id);
			quad->TextureTilingFactor = tile.TextureTilingFactor;
			quad->AspecRatio = (float)spec.mDimentions.x, (float)spec.mDimentions.y; // do we need this?

			m_Batch.QuadIndexCount += 6;

			++quad;
		}

		Render::GAPI::CommitRegion(region);

		if (m_Batch.QuadIndexCount)
		{
			constinit static const U32 params_alignment = Render::GAPI::GetOffsetAlignmentFor<Render::GAPI::DrawParams>();
			auto params_region = Render::GAPI::CreateRegion(sDrawParamsStream, sizeof(Render::GAPI::DrawParams), params_alignment);

			Render::GAPI::DrawParams* params = (Render::GAPI::DrawParams*)Render::GAPI::GetRegionLocation(params_region);

			params->Init();
			params->mFirstIndex = 0;
			params->mPrimitiveCount = m_Batch.QuadIndexCount / 3; // to do: use triangle count as counter

			Render::GAPI::CommitRegion(params_region);

			constinit static const U32 root_alignment = Render::GAPI::GetOffsetAlignmentFor<RootData>();
			auto root_region = Render::GAPI::CreateRegion(sDrawParamsStream, sizeof(RootData), root_alignment);
			RootData* root = (RootData*)Render::GAPI::GetRegionLocation(root_region);
			root->SceneDataIndex = Renderer::SceneData.IndexInStream;
			Render::GAPI::CommitRegion(root_region);

			Render::GAPI::SetRootDataOffset(sPipeline, Render::GAPI::GetRegionOffset(root_region));
			Render::GAPI::DrawIndirectCmd(sPipeline, Render::GAPI::GetRegionOffset(params_region));
		}


		//Flush();

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
			//BatchQuadDrawCall(sprite_comp.Sprite, transform, ID);
			//Flush();
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

		//ClearBatch();
	}

}