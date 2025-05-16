#include "FE_pch.h"
#include "Material.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"

namespace fe
{
	void ACMaterialData::Init()
	{
		ShaderID = 0;
		Uniforms.clear();
		TextureSlots.clear();
	}
	
	void Material::MakeMaterial(AssetID shaderID, const std::initializer_list<Uniform>& uniforms, const std::initializer_list<ShaderTextureSlot>& textureSlots)
	{
		auto& ACData = Get<ACMaterialData>();
		ACData.ShaderID = shaderID;
		ACData.Uniforms = uniforms;
		ACData.TextureSlots = textureSlots;
		ACData.UniformsDataSize = 0;
		for (const auto& uniform : uniforms)
		{
			ACData.UniformsDataSize += uniform.GetSize();
		}
	}

	void* Material::GetUniformValuePtr_Internal(const Uniform& targetUniform) const
	{
		FE_PROFILER_FUNC();

		auto& data = Get<ACMaterialData>();
		uint8_t* uniformDataPointer = (uint8_t*)(data.DefaultUniformsData);
		bool uniformFound = false;

		for (const auto& uniform : data.Uniforms)
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

	void* Material::GetUniformValuePtr_Internal(const std::string& name) const
	{
		FE_PROFILER_FUNC();

		auto& data = Get<ACMaterialData>();

		uint8_t* uniformDataPointer = (uint8_t*)(data.DefaultUniformsData);
		bool uniformFound = false;

		for (const auto& uniform : data.Uniforms)
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


	void Material::SetUniformValue(const Uniform& targetUniform, void* dataPointer)
	{
		FE_PROFILER_FUNC();

		auto& data = Get<ACMaterialData>();

		uint8_t* dest = (uint8_t*)(data.DefaultUniformsData);
		size_t uniformSize = 0;
		bool uniformFound = false;

		for (const auto& uniform : data.Uniforms)
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
		else // uniform initialization
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

			for (size_t i = 0; i < elements; i++)
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

	void Material::SetUniformValue(const std::string& name, void* dataPointer)
	{
		FE_PROFILER_FUNC();

		auto& data = Get<ACMaterialData>();

		uint8_t* dest = (uint8_t*)(data.DefaultUniformsData);
		size_t uniformSize = 0;
		bool uniformFound = false;

		for (const auto& uniform : data.Uniforms)
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
}