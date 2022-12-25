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
			m_UniformsDataSize += SDSizeOfType(uniform.GetType());
		}
		m_UniformsData = operator new(m_UniformsDataSize);
	}

	void MaterialInstance::SetUniformValue(const std::string& uniformName, void* dataPointer)
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

	void* MaterialInstance::GetUniformValuePtr(const std::string& uniformName)
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
}