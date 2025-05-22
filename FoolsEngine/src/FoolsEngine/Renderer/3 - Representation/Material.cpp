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
		auto& sm_data_component = shadingModelObserver.GetDataComponent();

		data_component.ShadingModelID = shadingModelObserver.GetID();

		auto& data = data_component.UniformsData;
		auto& size = data_component.UniformsDataSize;

		size = sm_data_component.UniformsDataSize;

		if (data)
			operator delete(data);
		data = operator new(size);

		std::memcpy(data, sm_data_component.DefaultUniformsData, size);

		auto& textures = data_component.Textures;
		textures.clear();
		textures.resize(sm_data_component.TextureSlots.size());
		for (auto& texture : textures)
		{
			texture = NullAssetID;
		}
	}

	void* MaterialObserver::GetUniformValuePtr_Internal(const ACMaterialData& dataComponent, const Uniform& targetUniform) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)(dataComponent.UniformsData);

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);
		for (const auto& uniform : shading_model_observer.GetDataComponent().Uniforms)
		{
			if (&targetUniform == &uniform)
			{
				return (void*)uniformDataPointer;
			}
			uniformDataPointer += uniform.GetSize();
		}

		FE_CORE_ASSERT(false, "Uniform not found in material!");
		return nullptr;
	}

	void* MaterialObserver::GetUniformValuePtr_Internal(const ACMaterialData& dataComponent, const std::string& name) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)(dataComponent.UniformsData);

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		for (const auto& uniform : shading_model_observer.GetDataComponent().Uniforms)
		{
			if (name == uniform.GetName())
			{
				return (void*)uniformDataPointer;
			}
			uniformDataPointer += uniform.GetSize();
		}

		FE_CORE_ASSERT(false, "Uniform not found in material!");
		return nullptr;
	}

	void MaterialUser::SetUniformValue(const ACMaterialData& dataComponent, const Uniform& targetUniform, void* dataPointer) const
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

	void MaterialUser::SetUniformValue(const ACMaterialData& dataComponent, const std::string& name, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		uint8_t* dest = (uint8_t*)(dataComponent.UniformsData);

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		for (const auto& uniform : shading_model_observer.GetDataComponent().Uniforms)
		{
			auto size = uniform.GetSize();
			if (name == uniform.GetName())
			{
				std::memcpy((void*)dest, dataPointer, size);
				return;
			}
			dest += size;
		}
		
		FE_CORE_ASSERT(false, "Uniform not found in material!");
	}

	AssetHandle<Texture2D> MaterialObserver::GetTexture(const ACMaterialData& dataComponent, const ShaderTextureSlot& textureSlot) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		auto slotsIt = shading_model_observer.GetDataComponent().TextureSlots.begin();
		for (const auto& texture : dataComponent.Textures)
		{
			if (slotsIt._Ptr == &textureSlot)
				return AssetHandle<Texture2D>(texture);

			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return AssetHandle<Texture2D>();
	}

	AssetHandle<Texture2D> MaterialObserver::GetTexture(const ACMaterialData& dataComponent, const std::string& textureSlotName) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		auto slotsIt = shading_model_observer.GetDataComponent().TextureSlots.begin();
		for (const auto& texture : dataComponent.Textures)
		{
			if (slotsIt->GetName() == textureSlotName)
				return AssetHandle<Texture2D>(texture);

			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return AssetHandle<Texture2D>();
	}

	void MaterialUser::SetTexture(ACMaterialData& dataComponent, const ShaderTextureSlot& textureSlot, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		auto slotsIt = shading_model_observer.GetDataComponent().TextureSlots.begin();
		for (auto& texture : dataComponent.Textures)
		{
			if (slotsIt._Ptr == &textureSlot)
				texture = textureID;

			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::SetTexture(ACMaterialData& dataComponent, const std::string& textureSlotName, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		auto slotsIt = shading_model_observer.GetDataComponent().TextureSlots.begin();
		for (auto& texture : dataComponent.Textures)
		{
			if (slotsIt->GetName() == textureSlotName)
				texture = textureID;

			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::ResetUniformValueToDefault(ACMaterialData& dataComponent, const Uniform& uniform) const
	{
		void* dest = GetUniformValuePtr_Internal(dataComponent, uniform);
		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		auto offset = (std::byte*)dest - (std::byte*)dataComponent.UniformsData;
		void* src = (std::byte*)shading_model_observer.GetDataComponent().DefaultUniformsData + offset;

		std::memcpy(dest, src, uniform.GetSize());
	}
}