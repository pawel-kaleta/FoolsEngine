#include "FE_pch.h"
#include "Material.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Assets\Serialization\ShaderDataSerialization.h"

namespace fe
{
	void ACMaterialCore::Init()
	{
		ShadingModelID = NullAssetID;
		Textures.clear();

		if (UniformsData) operator delete(UniformsData);
		UniformsData = nullptr;

		UniformsDataSize = 0;
	}

	void MaterialUser::MakeMaterial(const AssetObserver<ShadingModel>& shadingModelObserver) const
	{
		FE_PROFILER_FUNC();

		
		auto& core_component = GetCoreComponent();
		auto& sm_core_component = shadingModelObserver.GetCoreComponent();

		core_component.ShadingModelID = shadingModelObserver.GetID();

		auto& data = core_component.UniformsData;
		auto& size = core_component.UniformsDataSize;

		size = sm_core_component.UniformsDataSize;

		if (data)
			operator delete(data);
		data = operator new(size);

		std::memcpy(data, sm_core_component.DefaultUniformsData, size);

		auto& textures = core_component.Textures;
		textures.clear();
		textures.resize(sm_core_component.TextureSlots.size());
		for (auto& texture : textures)
		{
			texture = NullAssetID;
		}
	}

	void* MaterialObserver::GetUniformValuePtr_Internal(const ACMaterialCore& dataComponent, const Uniform& targetUniform) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)(dataComponent.UniformsData);

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);
		for (const auto& uniform : shading_model_observer.GetCoreComponent().Uniforms)
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

	void* MaterialObserver::GetUniformValuePtr_Internal(const ACMaterialCore& dataComponent, const std::string& name) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)(dataComponent.UniformsData);

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		for (const auto& uniform : shading_model_observer.GetCoreComponent().Uniforms)
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

	void MaterialUser::SetUniformValue(const ACMaterialCore& dataComponent, const Uniform& targetUniform, void* dataPointer) const
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

	void MaterialUser::SetUniformValue(const ACMaterialCore& dataComponent, const std::string& name, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		uint8_t* dest = (uint8_t*)(dataComponent.UniformsData);

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		for (const auto& uniform : shading_model_observer.GetCoreComponent().Uniforms)
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

	AssetHandle<Texture2D> MaterialObserver::GetTexture(const ACMaterialCore& dataComponent, const ShaderTextureSlot& textureSlot) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		auto slotsIt = shading_model_observer.GetCoreComponent().TextureSlots.begin();
		for (const auto& texture : dataComponent.Textures)
		{
			if (slotsIt._Ptr == &textureSlot)
				return AssetHandle<Texture2D>(texture);

			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return AssetHandle<Texture2D>();
	}

	AssetHandle<Texture2D> MaterialObserver::GetTexture(const ACMaterialCore& dataComponent, const std::string& textureSlotName) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		auto slotsIt = shading_model_observer.GetCoreComponent().TextureSlots.begin();
		for (const auto& texture : dataComponent.Textures)
		{
			if (slotsIt->GetName() == textureSlotName)
				return AssetHandle<Texture2D>(texture);

			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return AssetHandle<Texture2D>();
	}

	void MaterialUser::SetTexture(ACMaterialCore& dataComponent, const ShaderTextureSlot& textureSlot, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		auto slotsIt = shading_model_observer.GetCoreComponent().TextureSlots.begin();
		for (auto& texture : dataComponent.Textures)
		{
			if (slotsIt._Ptr == &textureSlot)
				texture = textureID;

			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::SetTexture(ACMaterialCore& dataComponent, const std::string& textureSlotName, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		auto slotsIt = shading_model_observer.GetCoreComponent().TextureSlots.begin();
		for (auto& texture : dataComponent.Textures)
		{
			if (slotsIt->GetName() == textureSlotName)
				texture = textureID;

			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::ResetUniformValueToDefault(ACMaterialCore& dataComponent, const Uniform& uniform) const
	{
		void* dest = GetUniformValuePtr_Internal(dataComponent, uniform);
		auto shading_model_observer = AssetObserver<ShadingModel>(dataComponent.ShadingModelID);

		auto offset = (std::byte*)dest - (std::byte*)dataComponent.UniformsData;
		void* src = (std::byte*)shading_model_observer.GetCoreComponent().DefaultUniformsData + offset;

		std::memcpy(dest, src, uniform.GetSize());
	}

	void Material::Serialize(const AssetObserver<Material>& assetObserver)
	{
		auto& core = assetObserver.GetCoreComponent();

		YAML::Emitter emitter;

		const auto shading_model_observer = AssetObserver<ShadingModel>(core.ShadingModelID);
		const auto& shading_model_core = shading_model_observer.GetCoreComponent();

		emitter << YAML::Key << "Shading Model" << YAML::Value << shading_model_observer.GetUUID();
		emitter << YAML::Key << "Uniforms Data Size" << YAML::Value << core.UniformsDataSize;
		emitter << YAML::Key << "Uniforms" << YAML::Value << YAML::BeginSeq;

		char* uniform_data_ptr = (char*)core.UniformsData;
		for (auto& uniform : shading_model_core.Uniforms)
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << uniform.GetName();
			emitter << YAML::Key << "Type" << YAML::Value << uniform.GetType().ToString();
			emitter << YAML::Key << "Count" << YAML::Value << uniform.GetCount();
			emitter << YAML::Key << "Value" << YAML::Value << YAML::BeginSeq;

			auto type = uniform.GetType();
			for (size_t i = 0; i < uniform.GetCount(); i++)
			{
				EmitShaderDataType(emitter, uniform_data_ptr, type);
				uniform_data_ptr += uniform.GetSize();
			}
			emitter << YAML::EndSeq;
		}
		emitter << YAML::EndSeq;

		emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;
		for (size_t i = 0; i < shading_model_core.TextureSlots.size(); ++i)
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Shader Texture Slot" << YAML::Value << shading_model_core.TextureSlots[i].GetName();
			const auto texture_observer = AssetObserver<Texture2D>(core.Textures[i]);
			emitter << YAML::Key << "Filepath" << YAML::Value << texture_observer.GetFilepath().string();
			emitter << YAML::Key << "UUID" << YAML::Value << texture_observer.GetUUID();
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndSeq;

		std::ofstream fout(assetObserver.GetFilepath());
		fout << emitter.c_str();
	}

	bool Material::Deserialize(AssetID assetID)
	{
		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		const auto& filepath = ECS_handle.get<ACFilepath>().Filepath;
		auto& core = ECS_handle.get<ACMaterialCore>();

		YAML::Node node = YAML::LoadFile(filepath.string());

		const auto& shading_model_node = node["Shading Model"];
		const auto& data_size_node     = node["Uniforms Data Size"];
		const auto& uniforms_node      = node["Uniforms"];
		const auto& textures_node      = node["Textures"];

		if (!shading_model_node) return false;
		if (!data_size_node) return false;
		if (!uniforms_node) return false;
		if (!textures_node) return false;
		if (!textures_node.IsSequence()) return false;

		auto shading_model_UUID = shading_model_node.as<UUID>();
		core.ShadingModelID = AssetManager::GetOrCreateAssetWithUUID(shading_model_UUID);

		core.UniformsDataSize = data_size_node.as<size_t>();
		core.UniformsData = operator new(core.UniformsDataSize);

		char* uniform_data_ptr = (char*)core.UniformsData;

		for (auto& uniform_node : uniforms_node)
		{
			const auto& name_node  = uniform_node["Name"];
			const auto& type_node  = uniform_node["Type"];
			const auto& count_node = uniform_node["Count"];
			const auto& value_node = uniform_node["Value"];

			if (!name_node) return false;
			if (!type_node) return false;
			if (!count_node) return false;
			if (!value_node) return false;

			const auto uniform_name = name_node.as<std::string>();
			// to do: compare (assert) with uniform's name in shading model
			ShaderData::Type uniform_type; uniform_type.FromInt(type_node.as<int>());
			const auto uniform_count = count_node.as<uint32_t>();

			if (!value_node.IsSequence() || value_node.size() != uniform_count) return false;
			auto uniform_size = ShaderData::SizeOfType(uniform_type);
			for (size_t i = 0; i < uniform_count; ++i)
			{
				bool success = LoadShaderDataType(value_node[i], uniform_data_ptr, uniform_type);
				if (!success) return false;
				uniform_data_ptr += uniform_size;
			}
		}

		for (auto& texture_node : textures_node)
		{
			const auto& texture_slot_node = texture_node["Shader Texture Slot"];
			const auto& texture_filepath_node = texture_node["Filepath"];
			const auto& texture_UUID_node = texture_node["UUID"];

			if (!texture_slot_node) return false;
			if (!texture_filepath_node) return false;
			if (!texture_UUID_node) return false;
			//To do: compare (assert) texture_slot_node with whader texture slot in shading model
			//To do: compare (assert) texture_filepath_node with filepath of texture with this UUID

			core.Textures.emplace_back(AssetManager::GetOrCreateAssetWithUUID(texture_UUID_node.as<UUID>()));
		}

		return true;
	}
}