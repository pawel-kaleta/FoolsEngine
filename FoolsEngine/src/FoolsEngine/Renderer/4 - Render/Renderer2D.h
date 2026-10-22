#pragma once

#include "FoolsEngine/Foundation/Math/Transform.h"
#include "FoolsEngine/Foundation/Utils/Core.h"

#include "FoolsEngine/Application/Time.h"

#include "FoolsEngine/Assets/AssetHandle.h"

#include "FoolsEngine/Renderer/2 - Representation/Shader.h"
#include "FoolsEngine/Renderer/2 - Representation/Texture.h"

#include <glm/glm.hpp>

namespace fe
{
	class Entity;
	class Camera;

	using AssetID = uint32_t;

	class Renderer2D
	{
	public:
		struct RenderStats
		{
			uint32_t Quads;
			uint32_t DrawCalls;
			Time::TimeStep RenderTime;
		};
		static RenderStats m_Stats;

		struct Quad
		{
			glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			float TextureTilingFactor = 1.0f;
			AssetHandle<Texture2D> Texture; // FlatWhite
		};

		static void RenderScene(const AssetObserver& scene);

		friend class Renderer;
		static void Startup();
		static void Init();
		static void Shutdown();

		struct ConstLimits {
			constexpr static U32 QuadsInBatch = 1000;
			constexpr static U32 MaxVeritices = QuadsInBatch * 4;
			constexpr static U32 MaxIndices = QuadsInBatch * 6;
			constexpr static U32 RendererTextureSlotsCount = 32;
			constexpr static U32 LayersCount = 19;
		};

		struct QuadVertex
		{
			glm::vec3 Shift;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			float TilingFactor;
			uint32_t TextureSampler;
			EntityID EntityID;
		};

		using QuadVerticesBatch = Array<QuadVertex, ConstLimits::MaxVeritices>;

		struct BatchData
		{
			Ref<QuadVerticesBatch> QuadVertices = CreateRef<QuadVerticesBatch>();
			QuadVertex* QuadVeriticesIt;
			uint32_t QuadIndexCount = 0;

			Array<AssetID, ConstLimits::RendererTextureSlotsCount> Textures;
			uint32_t TexturesCount = 1;
		};
		

		
		//ShaderTextureSlot m_BaseShaderTextureSlot;
		//RenderTextureSlotID m_BaseShaderSamplers[ConstLimits::RendererTextureSlotsCount];

		static void BeginScene();
		static void ClearBatch();
		static void BatchQuadDrawCall(const Quad& quad, const Transform& transform, EntityID ID);
		static void Flush();
		static void EndScene();

		
	};
}