#pragma once

#include "FoolsEngine\Renderer\6 - Resources\TextureLibrary.h"
#include "FoolsEngine\Renderer\6 - Resources\ShaderLibrary.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexArray.h"
#include "FoolsEngine\Renderer\3 - Representation\Transform.h"
#include "FoolsEngine\Renderer\3 - Representation\Camera.h"

#include "FoolsEngine\Core\Time.h"

namespace fe
{
	class Entity;
	class Scene;

	class Renderer2D
	{
	public:
		struct RenderStats
		{
			uint32_t Quads;
			uint32_t DrawCalls;
			Time::TimeStep RenderTime;
		};

		struct Quad
		{
			glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			float TextureTilingFactor = 1.0f;
			Ref<fe::Texture> Texture = TextureLibrary::Get("Base2DTexture");
		};

		static void Init();
		static void Shutdown() {}

		static void RenderScene(Scene& scene, Entity cameraSet);
		static void RenderScene(Scene& scene, const Camera& camera, const Transform& cameraTransform);

		static RenderStats GetStats() { return s_Stats; }

	private:
		struct ConstLimits {
			constexpr static uint32_t QuadsInBatch = 10000;
			constexpr static uint32_t MaxVeritices = QuadsInBatch * 4;
			constexpr static uint32_t MaxIndices = QuadsInBatch * 6;
			constexpr static uint32_t RendererTextureSlotsCount = 32;
			constexpr static uint32_t LayersCount = 19;
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

			BatchData Batch;

			Ref<Shader> BaseShader;
			ShaderTextureSlot BaseShaderTextureSlot;
			int32_t BaseShaderSamplers[ConstLimits::RendererTextureSlotsCount];
		};

		static Scope<Renderer2DData> s_Data;

		static void BeginScene(const glm::mat4& projection, const glm::mat4& view);
		static void ClearBatch();
		static void BatchQuadDrawCall(const Quad& quad, const Transform& transform);
		static void Flush();
		static void EndScene();

		static RenderStats s_Stats;
		static Time::TimePoint m_RenderStartTimePoint;
	};
}