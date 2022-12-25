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
		Material(std::shared_ptr<Shader> shader, const std::initializer_list<Uniform>& uniforms)
		{
			m_Shader = shader;
			m_Uniforms = uniforms;
		};
		const std::shared_ptr<Shader> GetShader() const { return m_Shader; }
		const std::vector<Uniform>& GetUniforms() const { return m_Uniforms; };
	private:
		std::shared_ptr<Shader> m_Shader;
		std::vector<Uniform> m_Uniforms;
	};

	class MaterialInstance
	{
	public:
		MaterialInstance(std::shared_ptr<Material> material)
		{
			FE_PROFILER_FUNC();

			m_Material = material;
			m_UniformsDataSize = 0;
			for (auto& uniform : m_Material->GetUniforms())
			{
				m_UniformsDataSize += SDSizeOfType(uniform.GetType());
			}
			m_UniformsData = operator new(m_UniformsDataSize);
		}

		void SetUniformValue(const std::string& uniformName, void* dataPointer)
		{
			FE_PROFILER_FUNC();

			uint8_t* dest = (uint8_t*)m_UniformsData;
			
			size_t uniformSize = 0;
			for (auto& uniform : m_Material->GetUniforms())
			{
				if (uniformName == uniform.GetName())
				{
					uniformSize = SDSizeOfType(uniform.GetType());
					break;
				}
				dest += SDSizeOfType(uniform.GetType());
			}
			std::memcpy((void*)dest, dataPointer, uniformSize);
		}

		void* GetUniformValuePtr(const std::string& uniformName)
		{
			FE_PROFILER_FUNC();

			if (m_UniformsDataSize == 0)
				return nullptr;

			uint8_t* uniformDataPointer = (uint8_t*)m_UniformsData;
			for (auto& uniform : m_Material->GetUniforms())
			{
				if (uniformName == uniform.GetName())
				{
					break;
				}
				uniformDataPointer += SDSizeOfType(uniform.GetType());
			}
			return (void*)uniformDataPointer;
		}

		~MaterialInstance()
		{
			FE_PROFILER_FUNC();
			operator delete(m_UniformsData);
		}

		std::shared_ptr<Material> GetMaterial() { return m_Material; }
	private:
		std::shared_ptr<Material> m_Material;
		void* m_UniformsData;
		size_t m_UniformsDataSize;
	};
}