#include "FE_pch.h"
#include "Material.h"

namespace fe
{
	MaterialInstance::MaterialInstance(Ref<Material> material, const std::string& name)
	{
		Init(material);
		m_Name = name;
	}

	void MaterialInstance::Init(Ref<Material> material)
	{
		FE_PROFILER_FUNC();

		m_Material = material;
		m_UniformsDataSize = 0;
		for (auto& uniform : m_Material->GetUniforms())
		{
			m_UniformsDataSize += uniform.GetSize();
		}
		if (m_UniformsData)
			operator delete(m_UniformsData);
		m_UniformsData = operator new(m_UniformsDataSize);

		for (auto& uniform : m_Material->GetUniforms())
		{
			this->SetUniformValue(uniform, nullptr);
		}

		m_Textures.clear();
		m_Textures.resize(m_Material->GetTextureSlots().size());
	}

	void* MaterialInstance::GetUniformValuePtr(const Uniform& targetUniform)
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)m_UniformsData;
		bool uniformFound = false;

		for (auto& uniform : m_Material->GetUniforms())
		{
			if (&targetUniform == &uniform)
			{
				uniformFound = true;
				break;
			}
			uniformDataPointer += uniform.GetSize();
		}

		if (!uniformFound)
		{
			FE_CORE_ASSERT(false, "Uniform not found in material!");
			return nullptr;
		}

		return (void*)uniformDataPointer;
	}
	
	void* MaterialInstance::GetUniformValuePtr(const std::string& name)
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)m_UniformsData;
		bool uniformFound = false;

		for (auto& uniform : m_Material->GetUniforms())
		{
			if (name == uniform.GetName())
			{
				uniformFound = true;
				break;
			}
			uniformDataPointer += uniform.GetSize();
		}
		if (!uniformFound)
		{
			FE_CORE_ASSERT(false, "Uniform not found in material!");
			return nullptr;
		}
		return (void*)uniformDataPointer;
	}

	void MaterialInstance::SetUniformValue(const Uniform& targetUniform, void* dataPointer)
	{
		FE_PROFILER_FUNC();

		uint8_t* dest = (uint8_t*)m_UniformsData;
		size_t uniformSize = 0;
		bool uniformFound = false;

		for (auto& uniform : m_Material->GetUniforms())
		{
			if (&targetUniform == &uniform)
			{
				uniformSize = uniform.GetSize();
				uniformFound = true;
				break;
			}
			dest += uniform.GetSize();
		}
		if (!uniformFound)
		{
			FE_CORE_ASSERT(false, "Uniform not found in material!");
			return;
		}

		if (dataPointer)
			std::memcpy((void*)dest, dataPointer, uniformSize);
		else
		{
			if (targetUniform.GetPrimitive() == ShaderData::Primitive::None)
			{
				FE_CORE_ASSERT(false, "Unknown Shader Data Primitive of uniform!");
				return;
			}
			if ((int)targetUniform.GetPrimitive() > 5)
			{
				FE_CORE_ASSERT(false, "Unknown Shader Data Primitive of uniform!");
				return;
			}

			auto type_size = ShaderData::SizeOfType(targetUniform.GetType());
			auto primitive_size = ShaderData::SizeOfPrimitive(targetUniform.GetPrimitive());
			auto elements_in_type = type_size / primitive_size;
			auto elements = elements_in_type * targetUniform.GetCount();

			for (int i = 0; i < elements; i++)
			{
				switch (targetUniform.GetPrimitive())
				{
				case ShaderData::Primitive::Bool:
					*(uint32_t*)dest = 0;
					dest += 4;
					break;
				case ShaderData::Primitive::Int:
					*(int32_t*)dest = 0;
					dest += 4;
					break;
				case ShaderData::Primitive::UInt:
					*(uint32_t*)dest = 0;
					dest += 4;
					break;
				case ShaderData::Primitive::Float:
					*(float*)dest = 0.0f;
					dest += 4;
					break;
				case ShaderData::Primitive::Double:
					*(float*)dest = 0.0;
					dest += 8;
					break;
				}
			}
		}
	}
	
	void MaterialInstance::SetUniformValue(const std::string& name, void* dataPointer)
	{
		FE_PROFILER_FUNC();

		uint8_t* dest = (uint8_t*)m_UniformsData;
		size_t uniformSize = 0;
		bool uniformFound = false;

		for (auto& uniform : m_Material->GetUniforms())
		{
			if (name == uniform.GetName())
			{
				uniformSize = uniform.GetSize();
				uniformFound = true;
				break;
			}
			dest += uniform.GetSize();
		}
		if (!uniformFound)
		{
			FE_CORE_ASSERT(false, "Uniform not found in material!");
			return;
		}
		std::memcpy((void*)dest, dataPointer, uniformSize);
	}

	Ref<Texture> MaterialInstance::GetTexture(const ShaderTextureSlot& textureSlot)
	{
		FE_PROFILER_FUNC();

		auto texturesIt = m_Textures.begin();
		auto slotsIt = m_Material->GetTextureSlots().begin();
		for (; texturesIt != m_Textures.end(); ++texturesIt, ++slotsIt)
		{
			if (slotsIt._Ptr == &textureSlot)
			{
				return *texturesIt;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return nullptr;
	}

	Ref<Texture> MaterialInstance::GetTexture(const std::string& textureSlotName)
	{
		FE_PROFILER_FUNC();

		auto texturesIt = m_Textures.begin();
		auto slotsIt = m_Material->GetTextureSlots().begin();
		for (; texturesIt != m_Textures.end(); ++texturesIt, ++slotsIt)
		{
			if (slotsIt->GetName() == textureSlotName)
			{
				return *texturesIt;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return nullptr;
	}

	void MaterialInstance::SetTexture(const ShaderTextureSlot& textureSlot, Ref<Texture> texture)
	{
		FE_PROFILER_FUNC();

		auto texturesIt = m_Textures.begin();
		auto slotsIt = m_Material->GetTextureSlots().begin();
		for (; texturesIt != m_Textures.end(); texturesIt++, slotsIt++)
		{
			if (slotsIt._Ptr == &textureSlot)
			{
				*texturesIt = texture;
				return;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialInstance::SetTexture(const std::string& textureSlotName, Ref<Texture> texture)
	{
		FE_PROFILER_FUNC();

		auto texturesIt = m_Textures.begin();
		auto slotsIt = m_Material->GetTextureSlots().begin();
		for (; texturesIt != m_Textures.end(); texturesIt++, slotsIt++)
		{
			if (slotsIt->GetName() == textureSlotName)
			{
				*texturesIt = texture;
				return;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

}