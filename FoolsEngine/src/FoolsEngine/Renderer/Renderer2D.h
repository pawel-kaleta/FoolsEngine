#pragma once
#include "RenderCommands.h"
#include "OrthographicCamera.h"
#include "Material.h"

namespace fe
{


	class Renderer2D
	{
	public:
		enum Layer
		{
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
			Layer Layer = Layer::L0;
			glm::vec2 Position = { 0.0f, 0.0f };
			glm::vec2 Size = { 1.0f, 1.0f };
			float TextureTilingFactor = 1.0f;
			glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			//in degrees
			void Rotation(float rotation) { m_Rotation = rotation; m_RotationActive = true; }
			void RotationOFF() { m_RotationActive = false; }
			void Texture(Ref<fe::Texture> texture) { m_Texture = texture; m_TextureActive = true; m_MaterialActive = false; }
			void TextureOFF() { m_TextureActive = false; }
			void Material(Ref<fe::MaterialInstance> materialInstance) { m_MaterialInstance = materialInstance; m_TextureActive = false; m_MaterialActive = true; }
			void MaterialOFF() { m_MaterialActive = false; }
		private:
			friend Renderer2D;
			bool m_RotationActive = false;
			bool m_TextureActive = false;
			bool m_MaterialActive = false;
			//in degrees
			float m_Rotation = 0.0f;
			Ref<fe::Texture> m_Texture;
			Ref<fe::MaterialInstance> m_MaterialInstance;
		};

		static void Init();
		static void Shutdown() {}

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void DrawQuad(const Quad& quad);
		//static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const uint32_t layer = 0);
		//static void DrawQuad(const glm::vec2& position, const glm::vec2& size, Ref<Texture> texture, const uint32_t layer = 0);
		//static void DrawQuad(const glm::vec2& position, const glm::vec2& size, Ref<MaterialInstance> materialInstance, const uint32_t layer = 0);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, Ref<fe::Texture> texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, Ref<MaterialInstance> materialInstance);

	private:
		struct Renderer2DData
		{
			Ref<VertexArray> QuadVertexArray;
			glm::mat4 VPMatrix;

			std::vector<Quad> UniqueQuads;

			Ref<Shader> BaseShader;
			ShaderTextureSlot BaseShaderTextureSlot;
			Ref<Texture> WhiteTexture;

			Ref<MaterialInstance> FlatColorMaterialInstance;
			Ref<MaterialInstance> BasicTextureMaterialInstance;
		};
		
		static Scope<Renderer2DData> s_Data;

		static void DrawUniqueQuad(const Quad& quad);
	};
}