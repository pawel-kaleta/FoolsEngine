#include "FE_pch.h"
#include "Material.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"

namespace fe
{
	void MaterialUser::MakeMaterial(const AssetObserver<ShadingModel>& shadingModelObserver) const
	{
		FE_PROFILER_FUNC();

		auto& data_component = Get<ACMaterialData>();

		data_component.ShadingModelID = shadingModelObserver.GetID();

		auto& data = data_component.UniformsData;
		auto& size = data_component.UniformsDataSize;

		if (data)
			operator delete(data);
		data = operator new(size);

		for (const auto& uniform : shadingModelObserver.GetUniforms())
		{
			this->SetUniformValue(uniform, nullptr);
		}

		auto& textures = data_component.Textures;
		textures.clear();
		textures.resize(shadingModelObserver.GetTextureSlots().size());
		for (auto& texture : textures)
		{
			texture = NullAssetID;
		}
	}

	void* MaterialObserver::GetUniformValuePtr_Internal(const Uniform& targetUniform) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)(Get<ACMaterialData>().UniformsData);
		bool uniformFound = false;

		for (const auto& uniform : GetShadingModel().Observe().GetUniforms())
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

	void* MaterialObserver::GetUniformValuePtr_Internal(const std::string& name) const
	{
		FE_PROFILER_FUNC();

		auto& data_component = Get<ACMaterialData>();

		uint8_t* uniformDataPointer = (uint8_t*)(data_component.UniformsData);
		bool uniformFound = false;

		 auto& shading_model_observer = AssetHandle<ShadingModel>(data_component.ShadingModelID).Observe();

		for (const auto& uniform : shading_model_observer.GetUniforms())
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

	void MaterialUser::SetUniformValue(const Uniform& targetUniform, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		auto& data_component = Get<ACMaterialData>();

		uint8_t* dest = (uint8_t*)(data_component.UniformsData);
		size_t uniformSize;
		
		auto& shading_model_observer = AssetHandle<ShadingModel>(data_component.ShadingModelID).Observe();

		bool uniformFound = false;
		for (const auto& uniform : shading_model_observer.GetUniforms())
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
			FE_CORE_ASSERT(false, "Uniform not found in shading model!");
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

	void MaterialUser::SetUniformValue(const std::string& name, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		auto& data_component = Get<ACMaterialData>();

		uint8_t* dest = (uint8_t*)(data_component.UniformsData);
		size_t uniformSize = 0;
		bool uniformFound = false;

		auto shading_model_observer = AssetHandle<ShadingModel>(data_component.ShadingModelID, LoadingPriority_None).Observe();

		for (const auto& uniform : shading_model_observer.GetUniforms())
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

	AssetHandle<Texture2D> MaterialObserver::GetTexture(const ShaderTextureSlot& textureSlot) const
	{
		FE_PROFILER_FUNC();

		auto& data_component = Get<ACMaterialData>();

		auto& textures = data_component.Textures;

		auto texturesIt = textures.begin();
		auto shading_model_observer = AssetHandle<ShadingModel>(data_component.ShadingModelID).Observe();
		auto slotsIt = shading_model_observer.GetTextureSlots().begin();
		for (; texturesIt != textures.end(); ++texturesIt, ++slotsIt)
		{
			if (slotsIt._Ptr == &textureSlot)
			{
				return AssetHandle<Texture2D>(*texturesIt);
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return AssetHandle<Texture2D>();
	}

	AssetHandle<Texture2D> MaterialObserver::GetTexture(const std::string& textureSlotName) const
	{
		FE_PROFILER_FUNC();

		auto& data_component = Get<ACMaterialData>();

		auto& textures = data_component.Textures;

		auto texturesIt = textures.begin();
		auto shading_model_observer = AssetHandle<ShadingModel>(data_component.ShadingModelID).Observe();
		auto slotsIt = shading_model_observer.GetTextureSlots().begin();
		for (; texturesIt != textures.end(); ++texturesIt, ++slotsIt)
		{
			if (slotsIt->GetName() == textureSlotName)
			{
				return AssetHandle<Texture2D>(*texturesIt);
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return AssetHandle<Texture2D>();
	}

	void MaterialUser::SetTexture(const ShaderTextureSlot& textureSlot, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		auto& data_component = Get<ACMaterialData>();

		auto& textures = data_component.Textures;

		auto texturesIt = textures.begin();
		auto shading_model_observer = AssetHandle<ShadingModel>(data_component.ShadingModelID).Observe();
		auto slotsIt = shading_model_observer.GetTextureSlots().begin();
		for (; texturesIt != textures.end(); texturesIt++, slotsIt++)
		{
			if (slotsIt._Ptr == &textureSlot)
			{
				*texturesIt = textureID;
				return;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::SetTexture(const std::string& textureSlotName, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		auto& data_component = Get<ACMaterialData>();

		auto& textures = data_component.Textures;

		auto texturesIt = textures.begin();
		auto material_observer = AssetHandle<ShadingModel>(data_component.ShadingModelID).Observe();
		auto slotsIt = material_observer.GetTextureSlots().begin();
		for (; texturesIt != textures.end(); texturesIt++, slotsIt++)
		{
			if (slotsIt->GetName() == textureSlotName)
			{
				*texturesIt = textureID;
				return;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}
	
}