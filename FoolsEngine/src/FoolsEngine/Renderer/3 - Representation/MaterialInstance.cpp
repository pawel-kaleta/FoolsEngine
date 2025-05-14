#include "FE_pch.h"
#include "MaterialInstance.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"

namespace fe
{
	void MaterialInstance::Init(const AssetObserver<Material>& materialObserver)
	{
		FE_PROFILER_FUNC();

		Get<ACMaterial>().MaterialID = materialObserver.GetID();
		auto& acUniformsData = Get<ACUniformsData>();
		auto& data = acUniformsData.UniformsData;
		auto& size = acUniformsData.UniformsDataSize;

		size = 0;
		for (auto& uniform : materialObserver.GetUniforms())
		{
			size += uniform.GetSize();
		}
		if (data)
			operator delete(data);
		data = operator new(size);

		for (auto& uniform : materialObserver.GetUniforms())
		{
			this->SetUniformValue(uniform, nullptr);
		}

		auto& textures = Get<ACTextures>().Textures;
		textures.clear();
		textures.resize(materialObserver.GetTextureSlots().size());
	}

	void MaterialInstance::MakeMaterialInstance(AssetUser<MaterialInstance>& miUser)
	{
		// TO DO: create a file

		miUser.Emplace<ACMaterial>();
		miUser.Emplace<ACUniformsData>();
		miUser.Emplace<ACTextures>();

		return;
	}

	void* MaterialInstance::GetUniformValuePtr_Internal(const Uniform& targetUniform) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)(Get<ACUniformsData>().UniformsData);
		bool uniformFound = false;

		for (auto& uniform : GetMaterial().Observe().GetUniforms())
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

	void* MaterialInstance::GetUniformValuePtr_Internal(const std::string& name) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)(Get<ACUniformsData>().UniformsData);
		bool uniformFound = false;

		auto material_observer = AssetHandle<Material>(Get<ACMaterial>().MaterialID).Observe();

		for (const auto& uniform : material_observer.GetUniforms())
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

		uint8_t* dest = (uint8_t*)(Get<ACUniformsData>().UniformsData);
		size_t uniformSize = 0;
		bool uniformFound = false;

		auto material_observer = AssetHandle<Material>(Get<ACMaterial>().MaterialID).Observe();

		for (auto& uniform : material_observer.GetUniforms())
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

	void MaterialInstance::SetUniformValue(const std::string& name, void* dataPointer)
	{
		FE_PROFILER_FUNC();

		uint8_t* dest = (uint8_t*)(Get<ACUniformsData>().UniformsData);
		size_t uniformSize = 0;
		bool uniformFound = false;

		auto material_observer = AssetHandle<Material>(Get<ACMaterial>().MaterialID, LoadingPriority_None).Observe();

		for (auto& uniform : material_observer.GetUniforms())
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

	AssetHandle<Texture2D> MaterialInstance::GetTexture(const ShaderTextureSlot& textureSlot) const
	{
		FE_PROFILER_FUNC();

		auto& textures = Get<ACTextures>().Textures;

		auto texturesIt = textures.begin();
		auto material_observer = AssetHandle<Material>(Get<ACMaterial>().MaterialID).Observe();
		auto slotsIt = material_observer.GetTextureSlots().begin();
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

	AssetHandle<Texture2D> MaterialInstance::GetTexture(const std::string& textureSlotName) const
	{
		FE_PROFILER_FUNC();

		auto& textures = Get<ACTextures>().Textures;

		auto texturesIt = textures.begin();
		auto material_observer = AssetHandle<Material>(Get<ACMaterial>().MaterialID).Observe();
		auto slotsIt = material_observer.GetTextureSlots().begin();
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

	void MaterialInstance::SetTexture(const ShaderTextureSlot& textureSlot, AssetID textureID)
	{
		FE_PROFILER_FUNC();

		auto& textures = Get<ACTextures>().Textures;

		auto texturesIt = textures.begin();
		auto material_observer = AssetHandle<Material>(Get<ACMaterial>().MaterialID).Observe();
		auto slotsIt = material_observer.GetTextureSlots().begin();
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

	void MaterialInstance::SetTexture(const std::string& textureSlotName, AssetID textureID)
	{
		FE_PROFILER_FUNC();

		auto& textures = Get<ACTextures>().Textures;

		auto texturesIt = textures.begin();
		auto material_observer = AssetHandle<Material>(Get<ACMaterial>().MaterialID).Observe();
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