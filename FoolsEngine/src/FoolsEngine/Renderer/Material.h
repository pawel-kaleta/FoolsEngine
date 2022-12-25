#pragma once

#include "APIAbstraction\Shader.h"

namespace fe
{
	class Uniform
	{
	public:
		Uniform(const std::string& name, SDType type)
			: m_Name(name), m_Type(type) {}

		const SDType& GetType() const { return m_Type; }
		const std::string& GetName() const { return m_Name; }
	private:
		std::string m_Name;
		SDType m_Type;
	};

	class Material
	{
	public:
		Material(Ref<Shader> shader, const std::initializer_list<Uniform>& uniforms)
			: m_Shader(shader), m_Uniforms(uniforms) {};

		const Ref<Shader> GetShader() const { return m_Shader; }
		const std::vector<Uniform>& GetUniforms() const { return m_Uniforms; };
	private:
		Ref<Shader> m_Shader;
		std::vector<Uniform> m_Uniforms;
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

		void SetUniformValue(const std::string& uniformName, void* dataPointer);
		void* GetUniformValuePtr(const std::string& uniformName);

		Ref<Material> GetMaterial() { return m_Material; }
	private:
		Ref<Material> m_Material;
		void* m_UniformsData;
		size_t m_UniformsDataSize;
	};
}