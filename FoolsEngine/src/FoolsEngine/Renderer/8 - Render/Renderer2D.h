#pragma once

#include "FoolsEngine\Math\Transform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"


#include "FoolsEngine\Core\Time.h"

#include "FoolsEngine\Assets\AssetHandle.h"

namespace fe
{
	class Scene;
	class Entity;
	class Framebuffer;
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

		struct Quad
		{
			glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			float TextureTilingFactor = 1.0f;
			AssetHandle<Texture2D> Texture = AssetHandle<Texture2D>((AssetID)BaseAssets::Textures2D::FlatWhite);
		};

		static void Init();
		static void Shutdown();

		static void RenderScene(Scene& scene, Entity cameraSet, Framebuffer& framebuffer);
		static void RenderScene(Scene& scene, const Camera& camera, const Transform& cameraTransform, Framebuffer& framebuffer);

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

			std::array<AssetHandle<Texture2D>, ConstLimits::RendererTextureSlotsCount> Textures;
			uint32_t TexturesCount = 1;
		};

		struct Renderer2DData
		{

			glm::mat4 VPMatrix;

			Ref<VertexBuffer> QuadVertexBuffer;

			BatchData Batch;

			AssetHandle<Shader> BaseShader;
			ShaderTextureSlot BaseShaderTextureSlot;
			RenderTextureSlotID BaseShaderSamplers[ConstLimits::RendererTextureSlotsCount];
		};

		static Renderer2DData& s_Data;

		static void BeginScene(const glm::mat4& projection, const glm::mat4& view, Framebuffer& framebuffer);
		static void ClearBatch();
		static void BatchQuadDrawCall(const Quad& quad, const Transform& transform, EntityID ID);
		static void Flush();
		static void EndScene(Framebuffer& framebuffer);

		static RenderStats s_Stats;
		static Time::TimePoint m_RenderStartTimePoint;
	};
}