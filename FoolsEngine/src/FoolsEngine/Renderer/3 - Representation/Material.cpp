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

		for (ShaderTextureSlot TS : m_Material->GetTextureSlots())
		{
			m_Textures.push_back(Ref<Texture>());
		}
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
		std::memcpy((void*)dest, dataPointer, uniformSize);
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
		auto slotsIT = m_Material->GetTextureSlots().begin();
		for (; texturesIt != m_Textures.end(); texturesIt++, slotsIT++)
		{
			if (slotsIT._Ptr == &textureSlot)
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
		auto slotsIT = m_Material->GetTextureSlots().begin();
		for (; texturesIt != m_Textures.end(); texturesIt++, slotsIT++)
		{
			if (slotsIT->GetName() == textureSlotName)
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
		auto slotsIT = m_Material->GetTextureSlots().begin();
		for (; texturesIt != m_Textures.end(); texturesIt++, slotsIT++)
		{
			if (slotsIT._Ptr == &textureSlot)
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
		auto slotsIT = m_Material->GetTextureSlots().begin();
		for (; texturesIt != m_Textures.end(); texturesIt++, slotsIT++)
		{
			if (slotsIT->GetName() == textureSlotName)
			{
				*texturesIt = texture;
				return;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	
}