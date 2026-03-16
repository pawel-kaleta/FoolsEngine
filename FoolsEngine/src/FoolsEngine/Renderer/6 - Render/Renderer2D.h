#pragma once

#include "FoolsEngine/Foundation/Math/Transform.h"

#include "FoolsEngine/Application/Time.h"

#include "FoolsEngine/Assets/AssetHandle.h"

#include "FoolsEngine/Renderer/2 - Resource/VertexArray.h"
#include "FoolsEngine/Renderer/2 - Resource/StaticBuffer.h"
#include "FoolsEngine/Renderer/4 - Representation/Shader.h"
#include "FoolsEngine/Renderer/4 - Representation/Texture.h"

namespace fe
{
	class Scene;
	class Entity;
	namespace Resource { struct FramebufferBase; }
	class Camera;

	using AssetID = uint32_t;

	class Renderer2D
	{
	public:
		static Renderer2D& Get() { return *s_Instance; }

		struct RenderStats
		{
			uint32_t Quads;
			uint32_t DrawCalls;
			Time::TimeStep RenderTime;
		};
		RenderStats m_Stats;

		struct Quad
		{
			glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			float TextureTilingFactor = 1.0f;
			AssetHandle<Texture2D> Texture; // FlatWhite
		};

		static void RenderScene(const AssetObserver<Scene>& scene);

	private:
		friend class Renderer;
		static void Startup();
		static void Init();
		static void Shutdown();

		struct ConstLimits {
			constexpr static uint32_t QuadsInBatch = 10000;
			constexpr static uint32_t MaxVeritices = QuadsInBatch * 4;
			constexpr static uint32_t MaxIndices = QuadsInBatch * 6;
			constexpr static uint32_t RendererTextureSlotsCount = 32;
			constexpr static uint32_t LayersCount = 19;
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

		using QuadVerticesBatch = std::array<QuadVertex, ConstLimits::MaxVeritices>;

		struct BatchData
		{
			Ref<QuadVerticesBatch> QuadVertices = CreateRef<QuadVerticesBatch>();
			QuadVerticesBatch::iterator QuadVeriticesIt;
			uint32_t QuadIndexCount = 0;

			std::array<AssetID, ConstLimits::RendererTextureSlotsCount> Textures;
			uint32_t TexturesCount = 1;
		};

		Ref<Resource::StaticBufferBase> m_QuadVertexBuffer;
		Ref<Resource::StaticBufferBase> m_QuadIndexBuffer;
		Ref<Resource::VertexArrayBase> m_VertexArray;
		//Ref<VertexBuffer> m_QuadVertexBuffer;

		BatchData m_Batch;

		AssetHandle<Shader> m_BaseShader;
		//ShaderTextureSlot m_BaseShaderTextureSlot;
		RenderTextureSlotID m_BaseShaderSamplers[ConstLimits::RendererTextureSlotsCount];

		static Renderer2D* s_Instance;

		void BeginScene();
		void ClearBatch();
		void BatchQuadDrawCall(const Quad& quad, const Transform& transform, EntityID ID);
		void Flush();
		void EndScene();

		Time::TimePoint m_RenderStartTimePoint;
	};
}