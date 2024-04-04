#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Core\UUID.h"

#include "FoolsEngine\Assets\AssetHandle.h"

namespace fe
{
	class Material : public Asset
	{
	public:
		Material(
			AssetSignature* signature,
			const std::string& name,
			const AssetHandle<Shader>& shader,
			const std::initializer_list<Uniform>& uniforms,
			const std::initializer_list<ShaderTextureSlot>& textureSlots)
			:
			Asset(signature),
			m_Name(name),
			m_Shader(shader),
			m_Uniforms(uniforms),
			m_TextureSlots(textureSlots)
		{ }

		static AssetType GetAssetType() { return AssetType::MaterialAsset; }

		//const Ref<Shader> GetShader() const { return m_Shader; }
		const AssetHandle<Shader> GetShader() const { return m_Shader; }
		const std::vector<Uniform>& GetUniforms() const { return m_Uniforms; }
		const std::vector<ShaderTextureSlot>& GetTextureSlots() const { return m_TextureSlots; }
		const std::string& GetName() const { return m_Name; }
		//UUID GetUUID() const { return m_UUID; }
	private:
		std::string m_Name;
		//Ref<Shader> m_Shader;
		AssetHandle<Shader> m_Shader;
		std::vector<Uniform> m_Uniforms;
		std::vector<ShaderTextureSlot> m_TextureSlots;
		//UUID m_UUID;
	};

	class MaterialInstance : public Asset
	{
	public:
		MaterialInstance() = default;
		MaterialInstance(const AssetHandle<Material>& material, const std::string& name);
		~MaterialInstance()
		{
			FE_PROFILER_FUNC();
			operator delete(m_UniformsData);
		}

		static AssetType GetAssetType() { return AssetType::MaterialInstanceAsset; }

		void Init(const AssetHandle<Material>& material);

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
		//UUID GetUUID() const { return m_UUID; }

		void* GetUniformValuePtr(const Uniform& targetUniform);
		void* GetUniformValuePtr(const std::string& name);
		void SetUniformValue(const Uniform& uniform, void* dataPointer);
		void SetUniformValue(const std::string& name, void* dataPointer);

		AssetHandle<Texture> GetTexture(const ShaderTextureSlot& textureSlot);
		AssetHandle<Texture> GetTexture(const std::string& textureSlotName);
		void SetTexture(const ShaderTextureSlot& textureSlot, AssetHandle<Texture> texture);
		void SetTexture(const std::string& textureSlotName, AssetHandle<Texture> texture);

		const std::vector<AssetHandle<Texture>>& GetTextures() const { return m_Textures; }
		//const Ref<Material> GetMaterial() const { return m_Material; }
		const AssetHandle<Material> GetMaterial() const { return m_Material; }
	private:
		std::string m_Name;
		//Ref<Material> m_Material;
		AssetHandle<Material> m_Material;
		void* m_UniformsData = nullptr;
		size_t m_UniformsDataSize = 0;
		std::vector<AssetHandle<Texture>> m_Textures;
		//UUID m_UUID;
	};

	
}