#include "FE_pch.h"
#include "Material.h"

namespace fe
{
	MaterialInstance::MaterialInstance(Ref<Material> material)
	{
		FE_PROFILER_FUNC();

		m_Material = material;
		m_UniformsDataSize = 0;
		for (auto& uniform : m_Material->GetUniforms())
		{
			m_UniformsDataSize += uniform.GetSize();
		}
		m_UniformsData = operator new(m_UniformsDataSize);
	}

	void MaterialInstance::SetUniformValue(const Uniform& targetUniform, void* dataPointer)
	{
		FE_PROFILER_FUNC();

		uint8_t* dest = (uint8_t*)m_UniformsData;

		size_t uniformSize = 0;
		for (auto& uniform : m_Material->GetUniforms())
		{
			if (&targetUniform == &uniform)
			{
				uniformSize = uniform.GetSize();
				break;
			}
			dest += uniform.GetSize();
		}
		std::memcpy((void*)dest, dataPointer, uniformSize);
	}

	void* MaterialInstance::GetUniformValuePtr(const Uniform& targetUniform)
	{
		FE_PROFILER_FUNC();

		if (m_UniformsDataSize == 0)
			return nullptr;

		uint8_t* uniformDataPointer = (uint8_t*)m_UniformsData;
		for (auto& uniform : m_Material->GetUniforms())
		{
			if (&targetUniform == &uniform)
			{
				break;
			}
			uniformDataPointer += uniform.GetSize();
		}
		return (void*)uniformDataPointer;
	}

	void MaterialInstance::SetUniformValue(const std::string& name, void* dataPointer)
	{
		FE_PROFILER_FUNC();

		uint8_t* dest = (uint8_t*)m_UniformsData;

		size_t uniformSize = 0;
		for (auto& uniform : m_Material->GetUniforms())
		{
			if (name == uniform.GetName())
			{
				uniformSize = uniform.GetSize();
				break;
			}
			dest += uniform.GetSize();
		}
		std::memcpy((void*)dest, dataPointer, uniformSize);
	}

	void* MaterialInstance::GetUniformValuePtr(const std::string& name)
	{
		FE_PROFILER_FUNC();

		if (m_UniformsDataSize == 0)
			return nullptr;

		uint8_t* uniformDataPointer = (uint8_t*)m_UniformsData;
		for (auto& uniform : m_Material->GetUniforms())
		{
			if (name == uniform.GetName())
			{
				break;
			}
			uniformDataPointer += uniform.GetSize();
		}
		return (void*)uniformDataPointer;
	}
}