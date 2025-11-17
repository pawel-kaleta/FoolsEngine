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

	void MaterialUser::MakeMaterial(const AssetObserver<ShadingModel>& shadingModelObserver) const { Material::MakeMaterial(this->GetID(), shadingModelObserver); }

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

			auto refs = texture_user.GetRefCounters();
			if (refs) //project asset
			{
				if (refs->LiveHandles[0].fetch_add(1) == 0)
				{
					if (!texture_user.IsLoaded())
					{
						TextureLoader::LoadTexture(texture_user);
						texture_user.CreateGDITexture2D(GDI);
						texture_user.UnloadFromCPU();

						texture_user.FlagLoaded();
					}
					
					texture_user.FlagLoadedAsDependency();
				}
			}
			else //internal asset
			{
				FE_CORE_ASSERT(!texture_user.IsLoadedAsDependency(), "Internal Texture already marked LoadedAsDependency during loading");
				FE_CORE_ASSERT(!texture_user.IsLoaded(), "Internal Texture already marked Loaded during loading");
				
				TextureLoader::LoadTexture(texture_user);
				texture_user.CreateGDITexture2D(GDI);
				texture_user.UnloadFromCPU();

				texture_user.FlagLoaded();
				texture_user.FlagLoadedAsDependency();
			}
			
		}
		
		FE_LOG_CORE_WARN("Material to gpu upload implementation not tested");
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

			auto refs = texture_user.GetRefCounters();
			if (refs) // project asset
			{
				if (refs->LiveHandles[0].fetch_sub(1) == 1)
					texture_user.ReleaseDependencyLoad();
			}
			else // internal asset
			{
				texture_user.ReleaseDependencyLoad();
			}
		}
	}

	void Material::SaveMetadata(YAML::Emitter& emitter, AssetID assetID)
	{
		auto assetObserver = AssetObserver<Material>(assetID);
		auto& core = assetObserver.GetCoreComponent();

		AssetObserver<ShadingModel> shading_model_observer(core.ShadingModelID);
		const auto& shading_model_core = shading_model_observer.GetCoreComponent();

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << assetObserver.GetUUID();
		emitter << YAML::Key << "Shading Model" << YAML::Value << shading_model_observer.GetUUID();
		emitter << YAML::Key << "Uniforms Data Size" << YAML::Value << core.UniformsDataSize;
		emitter << YAML::Key << "Uniforms" << YAML::Value << YAML::BeginMap;

		char* uniform_data_ptr = (char*)core.UniformsData;
		for (auto& uniform : shading_model_core.Uniforms)
		{
			emitter << YAML::Key << uniform.GetName() << YAML::Value << YAML::BeginMap;
			emitter << YAML::Key << "Type"  << YAML::Value << uniform.GetType().ToConstCharPtr();
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
		emitter << YAML::EndMap;

		emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginMap;
		for (size_t i = 0; i < shading_model_core.TextureSlots.size(); ++i)
		{
			emitter << YAML::Key << shading_model_core.TextureSlots[i].GetName() << YAML::Value;

			if (core.TextureIDs[i] != NullAssetID)
			{
				bool is_internal;
				{
					const AssetObserver<Texture2D> texture_observer(core.TextureIDs[i]);
					is_internal = AssetObserver<Texture2D>(core.TextureIDs[i]).AllOf<ACMasterAsset>();

					if (!is_internal)
					{
						emitter << YAML::BeginMap;
						emitter << YAML::Key << "Filepath" << YAML::Value << texture_observer.GetFilepath().string();
						emitter << YAML::Key << "UUID" << YAML::Value << texture_observer.GetUUID();
						emitter << YAML::EndMap;
					}
				}
				if (is_internal)
				{
					Texture2D::SaveMetadata(emitter, core.TextureIDs[i]);
				}
			}
			else
			{
				emitter << YAML::BeginMap;
				emitter << YAML::Key << "UUID" << YAML::Value << 0;
				emitter << YAML::EndMap;
			}

		}
		emitter << YAML::EndMap;
		emitter << YAML::EndMap;
	}

	static void LoadUniforms(const YAML::Node& node, void* uniformsData, const std::vector<Uniform>& uniforms)
	{
		FE_PROFILER_FUNC();

		char* uniform_data_ptr = (char*)uniformsData;

		for (const auto& uniform : uniforms)
		{
			const auto& uniform_node = node[uniform.GetName()];

			if (!uniform_node.IsDefined())
			{
				FE_LOG_CORE_WARN("Missing uniform in material definition");
				uniform_data_ptr += uniform.GetSize() * uniform.GetCount();
				continue;
			}
			else
			{
				const auto& type_node = uniform_node["Type"];
				const auto& count_node = uniform_node["Count"];
				const auto& value_node = uniform_node["Value"];

				if (!type_node ||
					!count_node ||
					!value_node)
				{
					FE_LOG_CORE_WARN("Ill defined uniform in material definition");
					uniform_data_ptr += uniform.GetSize() * uniform.GetCount();

					continue;
				}

				const auto uniform_count = count_node.as<uint32_t>();

				if (uniform.GetType().ToConstCharPtr() != type_node.as<std::string>() ||
					!value_node.IsSequence() ||
					value_node.size() != uniform_count ||
					uniform.GetCount() != uniform_count)
				{
					FE_LOG_CORE_WARN("Ill defined uniform '{0}' in material definition", uniform.GetName());
					uniform_data_ptr += uniform.GetSize() * uniform.GetCount();

					continue;
				}

				for (size_t i = 0; i < uniform.GetCount(); ++i)
				{
					bool success = LoadShaderDataType(value_node[i], uniform_data_ptr, uniform.GetType());
					if (!success) FE_LOG_CORE_WARN("Ill defined uniform '{0}' in material definition", uniform.GetName());
					uniform_data_ptr += uniform.GetSize();
				}
			}
		}
	}

	static void LoadTextures(const YAML::Node& node, std::vector<AssetID>& textureIDs, const std::vector<ShaderTextureSlot>& textureSlots, const std::filesystem::path& parentPath, AssetID master)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();

		for (size_t i = 0; i < textureSlots.size(); ++i)
		{
			const auto& texture_slot = textureSlots[i];
			const auto& texture_node = node[texture_slot.GetName()];

			if (!texture_node.IsDefined())
			{
				FE_LOG_CORE_WARN("Missing texture in material definition");
				continue;
			}

			const auto& texture_UUID_node = texture_node["UUID"];

			if (!texture_UUID_node.IsDefined())
			{
				FE_LOG_CORE_WARN("Misspecified texture in material definition");
				continue;
			}

			auto texture_UUID = texture_UUID_node.as<UUID>();

			if (texture_UUID == UUID(0)) continue;

			auto texture_id = AssetManager::GetOrCreateAssetWithUUID(texture_UUID);

			if (reg.all_of<ACRefsCounters>(texture_id)) //check if texture is master asset
			{
				textureIDs[i] = texture_id;
				//To do: compare (assert) texture_filepath_node with filepath of texture with this UUID
				continue;
			}

			const auto& usage_node = texture_node["Usage"];
			const auto& components_node = texture_node["Components"];
			const auto& format_node = texture_node["Format"];
			const auto& width_node = texture_node["Width"];
			const auto& height_node = texture_node["Height"];
			const auto& texture_source_filepath_node = texture_node["Source Filepath"];

			if (!usage_node.IsDefined() ||
				!components_node.IsDefined() ||
				!format_node.IsDefined() ||
				!width_node.IsDefined() ||
				!height_node.IsDefined() ||
				!texture_source_filepath_node.IsDefined())
			{
				FE_LOG_CORE_WARN("Misspecified texture in material definition");
				textureIDs[i] = NullAssetID;
				continue;
			}

			reg.emplace<ACAssetType>(texture_id).Type = AssetType::Texture2D;
			reg.emplace<ACMasterAsset>(texture_id).Master = master;
			auto& texture_core = reg.emplace<Texture2D::Core>(texture_id);
			texture_core.Init();

			auto& spec = texture_core.Specification;
			spec.Usage.FromString(usage_node.as<std::string>());
			spec.Components.FromString(components_node.as<std::string>());
			spec.Format.FromString(format_node.as<std::string>());
			spec.Width = width_node.as<uint32_t>();
			spec.Height = height_node.as<uint32_t>();

			AssetManager::SetSourcePath(texture_id, parentPath / texture_source_filepath_node.as<std::string>());
		}
	}

	bool Material::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();

		const auto& filepath = reg.get<ACFilepath>(assetID).Filepath;
		auto full_filepath = Project::GetInstance()->AssetsPath / filepath;

		YAML::Node node;

		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			node = YAML::LoadFile(full_filepath.string());
		}

		auto uuid_node = node["UUID"];
		if (uuid_node) // Base Assets don't have UUID in their file
		{
			if (reg.get<ACUUID>(assetID).UUID != node["UUID"].as<UUID>())
			{
				FE_CORE_ASSERT(false, "Not machting UUID in asset and its serialized node!");
				return false;
			}
		}
		else
		{
			FE_LOG_CORE_WARN("Missing UUID in Material serialized node");
		}

		const auto& shading_model_node = node["Shading Model"];
		const auto& data_size_node = node["Uniforms Data Size"];
		const auto& uniforms_node = node["Uniforms"];
		const auto& textures_node = node["Textures"];

		if (!shading_model_node ||
			!data_size_node ||
			!uniforms_node ||
			!textures_node)
		{
			FE_CORE_ASSERT(false, "Ill defined material");
			return false;
		}

		auto shading_model_UUID = shading_model_node.as<UUID>();
		auto shadingModelID = AssetManager::GetOrCreateAssetWithUUID(shading_model_UUID);

		AssetObserver<ShadingModel> sm_observer(shadingModelID);
		Material::MakeMaterial(assetID, sm_observer);
		auto& sm_core = sm_observer.GetCoreComponent();
		auto& core = reg.get<Material::Core>(assetID);

		LoadUniforms(uniforms_node, core.UniformsData, sm_core.Uniforms);

		auto& texture_slots = sm_core.TextureSlots;
		auto source_filepath = filepath.parent_path();

		LoadTextures(textures_node, core.TextureIDs, sm_core.TextureSlots, source_filepath, assetID);

		return true;
	}

	AssetID Material::LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();

		const auto& uuid_node = node["UUID"];
		if (!uuid_node)
		{
			FE_LOG_CORE_ERROR("Missing UUID in Material serialized node!");
			return NullAssetID;
		}

		auto uuid = uuid_node.as<UUID>();
		if (uuid == UUID(0))
		{
			FE_LOG_CORE_ERROR("Missing Material definition!");
			return NullAssetID;
		}

		auto asset_id = AssetManager::GetOrCreateAssetWithUUID(uuid);

		if (reg.all_of<ACRefsCounters>(asset_id)) return asset_id; // is ProjectAsset ?

		reg.emplace<ACAssetType>(asset_id).Type = AssetType::Material;
		reg.emplace<ACMasterAsset>(asset_id).Master = master;
		auto& core = reg.emplace<Material::Core>(asset_id);
		core.Init();

		const auto& shading_model_node = node["Shading Model"];
		if (!shading_model_node)
		{
			FE_LOG_CORE_ERROR("Ill defined material");
			return NullAssetID;
		}

		auto shading_model_UUID = shading_model_node.as<UUID>();
		auto shadingModelID = AssetManager::GetOrCreateAssetWithUUID(shading_model_UUID);

		AssetObserver<ShadingModel> sm_observer(shadingModelID);
		Material::MakeMaterial(asset_id, sm_observer);
		const auto& sm_core = sm_observer.GetCoreComponent();

		const auto& uniforms_node = node["Uniforms"];
		const auto& textures_node = node["Textures"];

		if (!uniforms_node ||
			!textures_node)
		{
			FE_LOG_CORE_ERROR("Ill defined material");
			return NullAssetID;
		}
		
		LoadUniforms(uniforms_node, core.UniformsData, sm_core.Uniforms);
		LoadTextures(textures_node, core.TextureIDs, sm_core.TextureSlots, parentPath, master);

		return true;
	}

	void Material::MakeMaterial(AssetID assetID, const AssetObserver<ShadingModel>& shadingModelObserver)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();

		auto& core_component = reg.get<Material::Core>(assetID);
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
}