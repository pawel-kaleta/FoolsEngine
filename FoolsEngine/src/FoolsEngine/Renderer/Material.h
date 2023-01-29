#pragma once

#include "APIAbstraction\Shader.h"
#include "APIAbstraction\Texture.h"

namespace fe
{
	class Material
	{
	public:
		Material(const std::string& name, Ref<Shader> shader, const std::initializer_list<Uniform>& uniforms, const std::initializer_list<ShaderTextureSlot>& textureSlots)
			: m_Name(name), m_Shader(shader), m_Uniforms(uniforms), m_TextureSlots(textureSlots) {}

		inline const Ref<Shader> GetShader() const { return m_Shader; }
		inline const std::vector<Uniform>& GetUniforms() const { return m_Uniforms; }
		inline const std::vector<ShaderTextureSlot>& GetTextureSlots() const { return m_TextureSlots; }
		inline const std::string& GetName() const { return m_Name; }
	private:
		std::string m_Name;
		Ref<Shader> m_Shader;
		std::vector<Uniform> m_Uniforms;
		std::vector<ShaderTextureSlot> m_TextureSlots;
	};

	class MaterialInstance
	{
	public:
		MaterialInstance(Ref<Material> material);
		~MaterialInstance()
		{
			FE_PROFILER_FUNC();
			operator delete(m_UniformsData);
		}

		void* GetUniformValuePtr(const Uniform& targetUniform);
		void* GetUniformValuePtr(const std::string& name);
		void SetUniformValue(const Uniform& uniform, void* dataPointer);
		void SetUniformValue(const std::string& name, void* dataPointer);

		Ref<Texture> GetTexture(const ShaderTextureSlot& textureSlot);
		Ref<Texture> GetTexture(const std::string& textureSlotName);
		void SetTexture(const ShaderTextureSlot& textureSlot, Ref<Texture> texture);
		void SetTexture(const std::string& textureSlotName, Ref<Texture> texture);

		inline const std::vector<Ref<Texture>>& GetTextures() const { return m_Textures; }
		inline const Ref<Material> GetMaterial() const { return m_Material; }
	private:
		Ref<Material> m_Material;
		void* m_UniformsData;
		size_t m_UniformsDataSize;
		std::vector<Ref<Texture>> m_Textures;
	};

	class MaterialLibrary
	{
	public:
		inline static void Add(Ref<Material> material) { s_ActiveInstance->IAdd(material); }

		inline static bool Exist(const std::string& name) { return s_ActiveInstance->IExist(name); }
		inline static Ref<Material> Get(const std::string& name) { return s_ActiveInstance->IGet(name); }

		inline static void SetActiveInstance(MaterialLibrary* materialLib) { s_ActiveInstance = materialLib; }

		void IAdd(const Ref<Material> material);

		bool IExist(const std::string& name) const;
		Ref<Material> IGet(const std::string& name);

	private:
		static MaterialLibrary* s_ActiveInstance;
		std::unordered_map<std::string, Ref<Material>> m_Materials;
	};
}