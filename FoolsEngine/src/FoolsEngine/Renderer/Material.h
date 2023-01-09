#pragma once

#include "APIAbstraction\Shader.h"

namespace fe
{
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

		void SetUniformValue(const Uniform& uniform, void* dataPointer);
		void* GetUniformValuePtr(const Uniform& targetUniform);

		void SetUniformValue(const std::string& name, void* dataPointer);
		void* GetUniformValuePtr(const std::string& name);

		Ref<Material> GetMaterial() { return m_Material; }
	private:
		Ref<Material> m_Material;
		void* m_UniformsData;
		size_t m_UniformsDataSize;
	};
}