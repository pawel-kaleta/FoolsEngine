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
	static Render::GAPI::GID sRootDataStream; // make this common for the whole rendering?
	static Render::GAPI::GID sPipeline;

	Renderer2D::RenderStats Renderer2D::m_Stats;

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

			sRootDataStream = Render::GAPI::CreateDownStream();
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
			
			Render::GAPI::SetBuffer(sPipeline, sRootDataStream, 0); // root data
			Render::GAPI::SetBuffer(sPipeline, Renderer::sSceneDataStream, 1);
			Render::GAPI::SetBuffer(sPipeline, sQuadsStream, 2);
			Render::GAPI::SetIndexSource(sPipeline, sIndexBuffer);
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
	}

	void Renderer2D::RenderScene(const AssetObserver& scene, const Render::Representation::Framebuffer& framebuffer)
	{
		FE_PROFILER_FUNC();

		constinit const static U32 quad_offset_align = Render::GAPI::GetOffsetAlignmentFor<QuadGPU>();
		constinit static const U32 params_alignment = Render::GAPI::GetOffsetAlignmentFor<Render::GAPI::DrawParams>();
		constinit static const U32 root_alignment = Render::GAPI::GetOffsetAlignmentFor<RootData>();
		
		BeginScene(framebuffer);

		auto& registry = scene.Get<Scene::Core>().GameplayWorld->m_Registry;

		{
			FE_PROFILER_SCOPE("Tiles");

			auto view_tiles = registry.view<CTile, CTransformGlobal>();

			UInt tiles_count = view_tiles.size_hint();
			UInt triangles_count = 0;

			Render::GAPI::GID tiles_region = Render::GAPI::CreateRegion(sQuadsStream, tiles_count * sizeof(QuadGPU), quad_offset_align);
			QuadGPU* quad = (QuadGPU*)Render::GAPI::GetRegionLocation(tiles_region);

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
				quad->Transform = *(Render::GAPI::std140_mat4x4*)&transform_mat;
				quad->Color = { tile.Color };
				quad->TextureView = Render::GAPI::GetTextureViewHandle(texture_view_id);
				quad->TextureTilingFactor = tile.TextureTilingFactor;
				quad->AspecRatio = (float)spec.mDimentions.x, (float)spec.mDimentions.y; // do we need this?

				triangles_count += 2;

				++quad;
			}

			Render::GAPI::CommitRegion(tiles_region);

			if (triangles_count)
			{
				auto root_region = Render::GAPI::CreateRegion(sRootDataStream, sizeof(RootData), root_alignment);
				RootData* root = (RootData*)Render::GAPI::GetRegionLocation(root_region);
				root->SceneDataIndex = Renderer::SceneData.IndexInStream;
				Render::GAPI::CommitRegion(root_region);

				Render::GAPI::SetRootDataIndex(sPipeline, Render::GAPI::GetRegionOffset(root_region)/sizeof(RootData));
				Render::GAPI::SetBlendSpec(sPipeline, { Render::GAPI::Blend::BlendFunction::None });
				Render::GAPI::ActivatePipelineCmd(sPipeline);
				Render::GAPI::DrawCmd(sPipeline, triangles_count, 0); // to do: use triangle count as counter
				Render::GAPI::RetireRegionCmd(root_region);

				m_Stats.Quads += triangles_count / 2;
				m_Stats.DrawCalls++;
			}

			Render::GAPI::RetireRegionCmd(tiles_region);
		}

		{
			FE_PROFILER_SCOPE("Sprites");

			registry.sort<CSprite>([&](const EntityID l, const EntityID r) {
				const auto& lPosition = registry.get<CTransformGlobal>(l).GetRef().Shift;
				const auto& rPosition = registry.get<CTransformGlobal>(r).GetRef().Shift;

				auto lDistance = glm::distance(Renderer::SceneData.CameraTransform.Shift, lPosition);
				auto rDistance = glm::distance(Renderer::SceneData.CameraTransform.Shift, rPosition);

				return lDistance > rDistance;
				});

			auto view_sprites = registry.view<CSprite, CTransformGlobal>();

			UInt sprite_count = view_sprites.size_hint();
			UInt triangles_count = 0;

			Render::GAPI::GID sprite_region = Render::GAPI::CreateRegion(sQuadsStream, sprite_count * sizeof(QuadGPU), quad_offset_align);
			QuadGPU* quad = (QuadGPU*)Render::GAPI::GetRegionLocation(sprite_region);

			for (auto ID : view_sprites)
			{
				auto [sprite_comp, entity_transform_comp] = view_sprites.get(ID);

				auto& sprite = sprite_comp.Sprite;

				if (!sprite.Texture.IsValid()) continue;
				if (sprite.Texture.GetLoadingPriority() == AssetLoadingPriority::None) continue;

				Transform transform = entity_transform_comp.GetRef() + sprite_comp.Offset;
				auto transform_mat = transform.GetMatrix();

				auto texture_observer = sprite.Texture.Observe();
				auto texture_view_id = texture_observer.Get<ACTexture2D_GPU<Render::GAPI::Platform::OpenGL>>().mTextureViewsGIDs[0]; // 0 is dogy, put this on tile?
				auto& spec = texture_observer.Get<Texture2D::Core>().mSpecification;

				quad->EntityID = ID;
				quad->Transform = *(Render::GAPI::std140_mat4x4*)&transform_mat;
				quad->Color = { sprite.Color };
				quad->TextureView = Render::GAPI::GetTextureViewHandle(texture_view_id);
				quad->TextureTilingFactor = sprite.TextureTilingFactor;
				quad->AspecRatio = (float)spec.mDimentions.x, (float)spec.mDimentions.y; // do we need this?

				triangles_count += 2;

				++quad;
			}

			Render::GAPI::CommitRegion(sprite_region);

			if (triangles_count)
			{
				auto root_region = Render::GAPI::CreateRegion(sRootDataStream, sizeof(RootData), root_alignment);
				RootData* root = (RootData*)Render::GAPI::GetRegionLocation(root_region);
				root->SceneDataIndex = Renderer::SceneData.IndexInStream;
				Render::GAPI::CommitRegion(root_region);

				Render::GAPI::SetRootDataIndex(sPipeline, Render::GAPI::GetRegionOffset(root_region) / sizeof(RootData));
				Render::GAPI::SetBlendSpec(sPipeline, { Render::GAPI::Blend::BlendFunction::OneMinusSourceAlpha });
				Render::GAPI::ActivatePipelineCmd(sPipeline);
				Render::GAPI::DrawCmd(sPipeline, triangles_count, 0); // to do: use triangle count as counter
				Render::GAPI::RetireRegionCmd(root_region);

				m_Stats.Quads += triangles_count / 2;
				m_Stats.DrawCalls++;
			}
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
}