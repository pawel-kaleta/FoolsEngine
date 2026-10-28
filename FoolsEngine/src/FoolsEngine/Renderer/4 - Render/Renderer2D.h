#pragma once

#include "FoolsEngine/Foundation/Math/Transform.h"
#include "FoolsEngine/Foundation/Utils/Core.h"

#include "FoolsEngine/Application/Time.h"

#include "FoolsEngine/Assets/AssetHandle.h"

#include "FoolsEngine/Renderer/2 - Representation/Shader.h"
#include "FoolsEngine/Renderer/2 - Representation/Texture.h"
#include "FoolsEngine/Renderer/2 - Representation/Framebuffer.h"


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
			U32 Quads;
			U32 DrawCalls;
			Time::TimeStep RenderTime;
		};
		static RenderStats m_Stats;

		struct Quad
		{
			glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			F32 TextureTilingFactor = 1.0f;
			AssetHandle<Texture2D> Texture; // FlatWhite
		};

		static void RenderScene(const AssetObserver& scene, const Render::Representation::Framebuffer& framebuffer);

		friend class Renderer;
		static void Startup();
		static void AcquireBaseAssets();
		static void Shutdown();

		struct ConstLimits {
			constexpr static U32 QuadsInBatch = 1000;
			constexpr static U32 MaxVeritices = QuadsInBatch * 4;
			constexpr static U32 MaxIndices = QuadsInBatch * 6;
		};

		struct QuadVertex
		{
			glm::vec3 Shift;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			F32 TilingFactor;
			U32 TextureSampler;
			EntityID EntityID;
		};
		

		static void BeginScene(const Render::Representation::Framebuffer& framebuffer);
		static void BatchQuadDrawCall(const Quad& quad, const Transform& transform, EntityID ID);
		static void Flush();
		static void EndScene();

		
	};
}