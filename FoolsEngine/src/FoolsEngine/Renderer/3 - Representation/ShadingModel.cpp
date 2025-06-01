#include "FE_pch.h"
#include "ShadingModel.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"

namespace fe
{
	void ACShadingModelCore::Init()
	{
		ShaderID = NullAssetID;
		Uniforms.clear();
		TextureSlots.clear();

		if (DefaultUniformsData) operator delete(DefaultUniformsData);
		DefaultUniformsData = nullptr;

		UniformsDataSize = 0;
	}
	
	void ShadingModelUser::MakeShadingModel(AssetID shaderID, const std::initializer_list<Uniform>& uniforms, const std::initializer_list<ShaderTextureSlot>& textureSlots) const
	{
		auto& ACData = Get<ACShadingModelCore>();
		ACData.ShaderID = shaderID;
		ACData.Uniforms = uniforms;
		ACData.TextureSlots = textureSlots;
		ACData.UniformsDataSize = 0;
		for (const auto& uniform : uniforms)
		{
			ACData.UniformsDataSize += uniform.GetSize();
		}

		if (ACData.DefaultUniformsData)
			operator delete(ACData.DefaultUniformsData);
		ACData.DefaultUniformsData = operator new(ACData.UniformsDataSize);

		uint8_t* uniformDataPointer = (uint8_t*)(ACData.DefaultUniformsData);
		for (const auto& uniform : uniforms)
		{
			InitUniformValue(uniform, uniformDataPointer);
			uniformDataPointer += uniform.GetSize();
		}

		// where is setting of default values for uniforms?
	}

	void* ShadingModelObserver::GetUniformValuePtr_Internal(const ACShadingModelCore& dataComponent, const Uniform& targetUniform) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)(dataComponent.DefaultUniformsData);

		for (const auto& uniform : dataComponent.Uniforms)
		{
			if (&targetUniform == &uniform)
			{
				return (void*)uniformDataPointer;
			}
			uniformDataPointer += uniform.GetSize();
		}

		FE_CORE_ASSERT(false, "Uniform not found in ShadingModel!");
		return nullptr;
	}

	void* ShadingModelObserver::GetUniformValuePtr_Internal(const ACShadingModelCore& dataComponent, const std::string& name) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)(dataComponent.DefaultUniformsData);

		for (const auto& uniform : dataComponent.Uniforms)
		{
			if (name == uniform.GetName())
			{
				return (void*)uniformDataPointer;
			}
			uniformDataPointer += uniform.GetSize();
		}

		FE_CORE_ASSERT(false, "Uniform not found in ShadingModel!");
		return nullptr;
	}

	void ShadingModelUser::SetUniformValue(const ACShadingModelCore& dataComponent, const Uniform& targetUniform, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		if (!dataPointer)
		{
			FE_CORE_ASSERT(false, "Pointer is null!");
			return;
		}

		void* dest = GetUniformValuePtr_Internal(dataComponent, targetUniform);
		std::memcpy((void*)dest, dataPointer, targetUniform.GetSize());
	}

	void ShadingModelUser::SetUniformValue(const ACShadingModelCore& dataComponent, const std::string& name, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		if (!dataPointer)
		{
			FE_CORE_ASSERT(false, "Pointer is null!");
			return;
		}

		uint8_t* dest = (uint8_t*)(dataComponent.DefaultUniformsData);

		for (const auto& uniform : dataComponent.Uniforms)
		{
			auto size = uniform.GetSize();
			if (name == uniform.GetName())
			{
				std::memcpy((void*)dest, dataPointer, size);
				return;
			}
			dest += size;
		}

		FE_CORE_ASSERT(false, "Uniform not found in ShadingModel!");
	}

	void ShadingModelUser::InitUniformValue(const Uniform& uniform, void* uniformDataPointer) const
	{
		if (uniform.GetPrimitive() == ShaderData::Primitive::None)
		{
			FE_CORE_ASSERT(false, "Unknown Shader Data Primitive of uniform!");
			return;
		}
		if (uniform.GetPrimitive().ToInt() > 5)
		{
			FE_CORE_ASSERT(false, "Unknown Shader Data Primitive of uniform!");
			return;
		}

		auto type_size = ShaderData::SizeOfType(uniform.GetType());
		auto primitive_size = ShaderData::SizeOfPrimitive(uniform.GetPrimitive());
		auto elements_in_type = type_size / primitive_size;
		auto elements = elements_in_type * uniform.GetCount();
		uint8_t* dest = (uint8_t*)(uniformDataPointer);

		for (size_t i = 0; i < elements; i++)
		{
			switch (uniform.GetPrimitive().Value)
			{
			case ShaderData::Primitive::Bool:
				*(uint32_t*)uniformDataPointer = 0;
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