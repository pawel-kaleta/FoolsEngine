#pragma once
#include "RenderCommands.h"
#include "OrthographicCamera.h"
#include "Material.h"
#include "FoolsEngine\Core\Time.h"

namespace fe
{

	class Renderer2D
	{
	public:
		struct RenderStats
		{
			uint32_t Quads;
			uint32_t DrawCalls;
			Time::TimeStep RenderTime;
		};

		enum Layer
		{
			L_9 = -9,
			L_8 = -8,
			L_7 = -7,
			L_6 = -6,
			L_5 = -5,
			L_4 = -4,
			L_3 = -3,
			L_2 = -2,
			L_1 = -1,
			L0 = 0,
			L1 = 1,
			L2 = 2,
			L3 = 3,
			L4 = 4,
			L5 = 5,
			L6 = 6,
			L7 = 7,
			L8 = 8,
			L9 = 9
		};

		class Quad
		{
		public:
			Layer Layer        = Layer::L0;
			glm::vec2 Position = { 0.0f, 0.0f };
			glm::vec2 Size     = { 1.0f, 1.0f };
			glm::vec4 Color    = { 1.0f, 1.0f, 1.0f, 1.0f };
			//in degrees
			float Rotation = 0.0f;
			//for optimization, can be left default
			bool Transparency = true;
			float TextureTilingFactor = 1.0f;
			Ref<fe::Texture> Texture = TextureLibrary::Get("Base2DTexture");
		};

		static void Init();
		static void Shutdown() {}

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void DrawQuad(const Quad& quad);

		inline static RenderStats GetStats() { return s_Stats; }

	private:
		struct ConstLimits {
			const static uint32_t QuadsInBatch = 10000;
			const static uint32_t MaxVeritices = QuadsInBatch * 4;
			const static uint32_t MaxIndices = QuadsInBatch * 6;
			const static uint32_t RendererTextureSlotsCount = 32;
			const static uint32_t LayersCount = 19;
		};

		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			float TilingFactor;
			uint32_t TextureSampler;
		};
		
		using QuadVerticesBatch = std::array<QuadVertex, ConstLimits::MaxVeritices>;

		struct BatchData
		{
			Ref<QuadVerticesBatch> QuadVertices = CreateRef<QuadVerticesBatch>();
			QuadVerticesBatch::iterator QuadVeriticesIt;
			uint32_t QuadIndexCount = 0;

			std::array<Ref<Texture>, ConstLimits::RendererTextureSlotsCount> Textures;
			uint32_t TexturesCount = 1;
		};

		struct Renderer2DData
		{
			glm::mat4 VPMatrix;

			Ref<VertexArray>  QuadVertexArray;
			Ref<VertexBuffer> QuadVertexBuffer;

			BatchData OpaqueBatch;
			BatchData TransparentBatches[19];

			Ref<Shader> BaseShader;
			ShaderTextureSlot BaseShaderTextureSlot;
			int32_t BaseShaderSamplers[ConstLimits::RendererTextureSlotsCount];
		};
		
		static Scope<Renderer2DData> s_Data;

		static void BatchQuadDrawCall(const Quad& quad, BatchData& batch);
		static void Flush(BatchData& batch, bool transparency);

		static RenderStats s_Stats;
		static Time::TimePoint m_RenderStartTimePoint;
	};


}