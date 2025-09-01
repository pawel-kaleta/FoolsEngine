#include "FE_pch.h"
#include "Material.h"

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"

#include "FoolsEngine\Core\Project.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Assets\Serialization\ShaderDataSerialization.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"

namespace fe
{
	void ACMaterialCore::Init()
	{
		TextureIDs.clear();

		if (UniformsData) operator delete(UniformsData);
		UniformsData = nullptr;

		UniformsDataSize = 0;
		ShadingModelID = NullAssetID;
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

		auto& textures = core_component.TextureIDs;
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

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);
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

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

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

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

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

	AssetID MaterialObserver::GetTextureID(const ACMaterialCore& dataComponent, const ShaderTextureSlot& textureSlot) const
	{
		FE_PROFILER_FUNC();

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		const auto& texture_slots = shading_model_observer.GetCoreComponent().TextureSlots;
		for (size_t i = 0; i < texture_slots.size(); i++)
		{
			if (&(texture_slots[i]) == &textureSlot)
				return dataComponent.TextureIDs[i];
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return NullAssetID;
	}

	AssetID MaterialObserver::GetTextureID(const ACMaterialCore& dataComponent, const std::string& textureSlotName) const
	{
		FE_PROFILER_FUNC();

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		const auto& texture_slots = shading_model_observer.GetCoreComponent().TextureSlots;
		for (size_t i = 0; i < texture_slots.size(); i++)
		{
			if (texture_slots[i].GetName() == textureSlotName)
				return dataComponent.TextureIDs[i];
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return NullAssetID;
	}

	void MaterialUser::SetTexture(ACMaterialCore& dataComponent, const ShaderTextureSlot& textureSlot, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		const auto& texture_slots = shading_model_observer.GetCoreComponent().TextureSlots;
		for (size_t i = 0; i < texture_slots.size(); i++)
		{
			if (&(texture_slots[i]) == &textureSlot)
			{
				dataComponent.TextureIDs[i] = textureID;
				return;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::SetTexture(ACMaterialCore& dataComponent, const std::string& textureSlotName, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		auto slotsIt = shading_model_observer.GetCoreComponent().TextureSlots.begin();
		const auto& texture_slots = shading_model_observer.GetCoreComponent().TextureSlots;
		for (size_t i = 0; i < texture_slots.size(); i++)
		{
			if (texture_slots[i].GetName() == textureSlotName)
			{
				dataComponent.TextureIDs[i] = textureID;
				return;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::ResetUniformValueToDefault(ACMaterialCore& dataComponent, const Uniform& uniform) const
	{
		void* dest = GetUniformValuePtr_Internal(dataComponent, uniform);
		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		auto offset = (std::byte*)dest - (std::byte*)dataComponent.UniformsData;
		void* src = (std::byte*)shading_model_observer.GetCoreComponent().DefaultUniformsData + offset;

		std::memcpy(dest, src, uniform.GetSize());
	}

	bool MaterialUser::SendDataToGPU(GDIType GDI) const
	{
		auto& core = Get<ACMaterialCore>();
		
		if (core.ShadingModelID == NullAssetID)
			return false;

		for (const auto& texture_ID : core.TextureIDs)
		{
			if (texture_ID == NullAssetID)
				return false;

			AssetUser<Texture2D> texture_user(texture_ID);
			
			FE_CORE_ASSERT(texture_user.GetType() == AssetType::Texture2D, "Trying to load texture in material that is not a texture.");

			TextureLoader::LoadTexture(texture_user);
			texture_user.CreateGDITexture2D(GDI);
			texture_user.UnloadFromCPU();
			texture_user.FlagLoadedAsDependency();
			texture_user.FlagLoaded();
		}
		
		FE_LOG_CORE_ERROR("Material to gpu upload not implemented... I think...");
		return false;
	}

	void MaterialUser::Release() const
	{
		auto& core = Get<ACMaterialCore>();

		for (const auto& texture_ID : core.TextureIDs)
		{
			if (texture_ID == NullAssetID)
				continue;

			AssetUser<Texture2D> texture_user(texture_ID);
			texture_user.ReleaseDependencyLoad();
		}
	}

	void Material::SaveMetadata(YAML::Emitter& emitter, AssetID assetID)
	{
		auto assetObserver = AssetObserver<Material>(assetID);
		auto& core = assetObserver.GetCoreComponent();

		AssetObserver<ShadingModel> shading_model_observer(core.ShadingModelID);
		const auto& shading_model_core = shading_model_observer.GetCoreComponent();

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Shading Model" << YAML::Value << shading_model_observer.GetUUID();
		emitter << YAML::Key << "Uniforms Data Size" << YAML::Value << core.UniformsDataSize;
		emitter << YAML::Key << "Uniforms" << YAML::Value << YAML::BeginSeq;

		char* uniform_data_ptr = (char*)core.UniformsData;
		for (auto& uniform : shading_model_core.Uniforms)
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << uniform.GetName();
			emitter << YAML::Key << "Type" << YAML::Value << uniform.GetType().ToConstCharPtr();
			emitter << YAML::Key << "Count" << YAML::Value << uniform.GetCount();
			emitter << YAML::Key << "Value" << YAML::Value << YAML::BeginSeq;

			auto type = uniform.GetType();
			for (size_t i = 0; i < uniform.GetCount(); i++)
			{
				float* test = (float*)uniform_data_ptr;
				EmitShaderDataType(emitter, uniform_data_ptr, type);
				uniform_data_ptr += uniform.GetSize();
			}
			emitter << YAML::EndSeq;
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndSeq;

		emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;
		for (size_t i = 0; i < shading_model_core.TextureSlots.size(); ++i)
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Shader Texture Slot" << YAML::Value << shading_model_core.TextureSlots[i].GetName();
			if (core.TextureIDs[i] != NullAssetID)
			{
				const AssetObserver<Texture2D> texture_observer(core.TextureIDs[i]);
				emitter << YAML::Key << "Filepath" << YAML::Value << texture_observer.GetFilepath().string();
				emitter << YAML::Key << "UUID" << YAML::Value << texture_observer.GetUUID();
			}
			else
			{
				emitter << YAML::Key << "Filepath" << YAML::Value << "";
				emitter << YAML::Key << "UUID" << YAML::Value << 0;
			}
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndSeq;
		emitter << YAML::EndMap;

		//std::ofstream fout(Project::GetInstance()->AssetsPath / assetObserver.GetFilepath());
		//fout << emitter.c_str();
	}

	bool Material::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		auto& core = ECS_handle.get<ACMaterialCore>();

		auto filepath = Project::GetInstance()->AssetsPath;
		filepath /= ECS_handle.get<ACFilepath>().Filepath;
		YAML::Node node = YAML::LoadFile(filepath.string());

		auto uuid_node = node["UUID"];
		if (uuid_node) // Base Assets don't have UUID in their file
		{
			if (ECS_handle.get<ACUUID>().UUID != node["UUID"].as<UUID>())
			{
				FE_CORE_ASSERT(false, "Not machting UUID in asset and its metafile!");
				return false;
			}
		}
		else
		{
			FE_LOG_CORE_WARN("Missing UUID in Material file");
		}

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
			FE_PROFILER_SCOPE("Uniform");
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
			ShaderData::Type uniform_type; uniform_type.FromString(type_node.as<std::string>());
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
			FE_PROFILER_SCOPE("Texture");
			const auto& texture_slot_node = texture_node["Shader Texture Slot"];
			const auto& texture_filepath_node = texture_node["Filepath"];
			const auto& texture_UUID_node = texture_node["UUID"];

			if (!texture_slot_node) return false;
			if (!texture_filepath_node) return false;
			if (!texture_UUID_node) return false;
			//To do: compare (assert) texture_slot_node with texture slot in shading model
			//To do: compare (assert) texture_filepath_node with filepath of texture with this UUID

			if (texture_UUID_node.as<UUID>() == UUID(0))
				core.TextureIDs.emplace_back(NullAssetID);
			else
				core.TextureIDs.emplace_back(AssetManager::GetOrCreateAssetWithUUID(texture_UUID_node.as<UUID>()));
		}

		return true;
	}
}