#include "FE_pch.h"
#include "Material.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Assets/Loaders/TextureLoader.h"
#include "FoolsEngine/Assets/Serialization/YAML.h"
#include "FoolsEngine/Assets/Serialization/GPUDataSerialization.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"

#include "FoolsEngine/Foundation/Memory/Pile.h"

namespace fe
{
	void ACMaterialCore::Init()
	{
		ShadingModelID = NullAssetID;
	}

	void* MaterialObserver::GetUniformValuePtr_Internal(const ACMaterialCore& dataComponent, const Description::Buffer::Element& targetUniform) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniform_data_pointer = (uint8_t*)(dataComponent.UniformsData.Elements);

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);
		auto& uniforms = shading_model_observer.GetUniforms();

		for (const auto& uniform : uniforms.Elements)
		{
			if (&targetUniform == &uniform)
			{
				return (void*)uniform_data_pointer;
			}
			uniform_data_pointer += uniform.Size() * uniform.Count;
		}

		FE_CORE_ASSERT(false, "Uniform not found in material!");
		return nullptr;
	}

	void* MaterialObserver::GetUniformValuePtr_Internal(const ACMaterialCore& dataComponent, String name) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniform_data_pointer = (uint8_t*)(dataComponent.UniformsData.Elements);

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);
		auto& uniforms = shading_model_observer.GetUniforms();

		for (const auto& uniform : uniforms.Elements)
		{
			if (CompareStringsEqual(name, uniform.Name))
			{
				return (void*)uniform_data_pointer;
			}
			uniform_data_pointer += uniform.Size() * uniform.Count;
		}

		FE_CORE_ASSERT(false, "Uniform not found in material!");
		return nullptr;
	}

	void MaterialUser::MakeMaterial(const AssetObserver<ShadingModel>& shadingModelObserver) const { Material::MakeMaterial(this->GetID(), shadingModelObserver); }

	void MaterialUser::SetUniformValue(const ACMaterialCore& dataComponent, const Description::Buffer::Element& targetUniform, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		if (!dataPointer)
		{
			FE_CORE_ASSERT(false, "Pointer is null!");
			return;
		}

		void* dest = GetUniformValuePtr_Internal(dataComponent, targetUniform);
		std::memcpy((void*)dest, dataPointer, targetUniform.Size() * targetUniform.Count);
	}

	void MaterialUser::SetUniformValue(const ACMaterialCore& dataComponent, String name, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		uint8_t* dest = (uint8_t*)(dataComponent.UniformsData.Elements);

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		for (const auto& uniform : shading_model_observer.GetUniforms().Elements)
		{
			auto size = uniform.Size();
			auto count = uniform.Count;
			if (CompareStringsEqual(name, uniform.Name))
			{
				std::memcpy((void*)dest, dataPointer, size * count);
				return;
			}
			dest += size * count;
		}
		
		FE_CORE_ASSERT(false, "Uniform not found in material!");
	}

	AssetID MaterialObserver::GetTextureID(const ACMaterialCore& dataComponent, const Description::ShaderInterface::TextureSampler& textureSampler) const
	{
		FE_PROFILER_FUNC();

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		const auto& program_spec_id = shading_model_observer.GetCore().ProgramSpecificationID;
		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[program_spec_id];
		const auto& texture_sampler_ids = program_spec.TextureSamplerIDs;
		for (size_t i = 0; i < texture_sampler_ids.Count; i++)
		{
			if (&(library.TextureSamplers[texture_sampler_ids[i]]) == &textureSampler)
				return dataComponent.TextureIDs[i];
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return NullAssetID;
	}

	AssetID MaterialObserver::GetTextureID(const ACMaterialCore& dataComponent, String textureSamplerName) const
	{
		FE_PROFILER_FUNC();

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		const auto& program_spec_id = shading_model_observer.GetCore().ProgramSpecificationID;
		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[program_spec_id];
		const auto& texture_sampler_ids = program_spec.TextureSamplerIDs;
		for (size_t i = 0; i < texture_sampler_ids.Count; i++)
		{
			const auto& texture_sampler = library.TextureSamplers[texture_sampler_ids[i]];
			if (CompareStringsEqual(texture_sampler.Name, textureSamplerName))
				return dataComponent.TextureIDs[i];
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return NullAssetID;
	}

	size_t MaterialObserver::GetCPUDataSize() const
	{
		size_t result = 0;
		const auto& core = GetCore();
		
		AssetObserver<ShadingModel> shading_model_observer(core.ShadingModelID);
		if (shading_model_observer.IsMaster()) result += shading_model_observer.GetCPUDataSize();
		
		result += core.UniformsData.Count;

		return result;
	}

	void MaterialUser::SetTexture(ACMaterialCore& dataComponent, const Description::ShaderInterface::TextureSampler& textureSampler, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		const auto& program_spec_id = shading_model_observer.GetCore().ProgramSpecificationID;
		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[program_spec_id];
		const auto& texture_sampler_ids = program_spec.TextureSamplerIDs;
		for (size_t i = 0; i < texture_sampler_ids.Count; i++)
		{
			if (&(library.TextureSamplers[texture_sampler_ids[i]]) == &textureSampler)
			{
				dataComponent.TextureIDs[i] = textureID;
				return;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::SetTexture(ACMaterialCore& dataComponent, String textureSamplerName, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		const auto& program_spec_id = shading_model_observer.GetCore().ProgramSpecificationID;
		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[program_spec_id];
		const auto& texture_sampler_ids = program_spec.TextureSamplerIDs;
		for (size_t i = 0; i < texture_sampler_ids.Count; i++)
		{
			const auto& texture_sampler = library.TextureSamplers[texture_sampler_ids[i]];
			if (CompareStringsEqual(texture_sampler.Name, textureSamplerName))
			{
				dataComponent.TextureIDs[i] = textureID;
				return;
			}
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::ResetUniformValueToDefault(ACMaterialCore& dataComponent, const Description::Buffer::Element& uniform) const
	{
		void* dest = GetUniformValuePtr_Internal(dataComponent, uniform);
		AssetObserver<ShadingModel> shading_model_observer(dataComponent.ShadingModelID);

		auto offset = (std::byte*)dest - (std::byte*)dataComponent.UniformsData.Elements;
		void* src = (std::byte*)shading_model_observer.GetCore().DefaultUniformsData + offset;

		std::memcpy(dest, src, uniform.Size() * uniform.Count);
	}

	bool MaterialUser::SendDataToGPU(GAPIType GAPI) const
	{
		auto& core = Get<ACMaterialCore>();

		if (core.ShadingModelID == NullAssetID)
			return false;

		//if (AllOf<ACGPUBuffer>())
		//{
		//	FE_CORE_ASSERT(false, "Already on GPU");
		//	return false;
		//}

		//auto& buffer_comp = Emplace<ACGPUBuffer>();
		//buffer_comp.Buffer.Usage = Description::Buffer::Usage::ShaderStorage;
		//buffer_comp.Buffer.Create();
		//
		//buffer_comp.Buffer.Upload(GetGPUDataSize(), nullptr);
		//
		//return SendDataToGPUInternal(GAPI, &buffer_comp.Buffer, 0);

		return true;
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
		auto asset_observer = AssetObserver<Material>(assetID);
		auto& core = asset_observer.GetCore();

		AssetObserver<ShadingModel> shading_model_observer(core.ShadingModelID);
		const auto& shading_model_core = shading_model_observer.GetCore();

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << asset_observer.GetUUID();
		emitter << YAML::Key << "Shading Model" << YAML::Value << shading_model_observer.GetUUID();
		emitter << YAML::Key << "Uniforms Data Size" << YAML::Value << core.UniformsData.Count;
		emitter << YAML::Key << "Uniforms" << YAML::Value << YAML::BeginMap;

		char* uniform_data_ptr = (char*)core.UniformsData.Elements;

		for (auto& uniform : shading_model_observer.GetUniforms().Elements)
		{
			emitter << YAML::Key << uniform.Name.CData() << YAML::Value << YAML::BeginMap;
			emitter << YAML::Key << "Type"  << YAML::Value << uniform.Type.ToConstCharPtr();
			emitter << YAML::Key << "Count" << YAML::Value << uniform.Count;
			emitter << YAML::Key << "Value" << YAML::Value << YAML::BeginSeq;

			for (size_t i = 0; i < uniform.Count; i++)
			{
				float* test = (float*)uniform_data_ptr;
				EmitGPUDataType(emitter, uniform_data_ptr, uniform.Type);
				uniform_data_ptr += uniform.Size();
			}
			emitter << YAML::EndSeq;
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndMap;

		emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginMap;

		const auto& program_spec_id = shading_model_observer.GetCore().ProgramSpecificationID;
		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[program_spec_id];
		const auto& texture_sampler_ids = program_spec.TextureSamplerIDs;

		Pile p;
		STD_PMR_Allocator pmr_p(&p);
		for (size_t i = 0; i < texture_sampler_ids.Count; ++i)
		{
			emitter << YAML::Key << library.TextureSamplers[texture_sampler_ids[i]].Name.CData() << YAML::Value;

			if (core.TextureIDs[i] != NullAssetID)
			{
				bool is_internal;
				{
					const AssetObserver<Texture2D> texture_observer(core.TextureIDs[i]);
					is_internal = AssetObserver<Texture2D>(core.TextureIDs[i]).AllOf<ACMasterAsset>();

					if (!is_internal)
					{
						emitter << YAML::BeginMap;
						emitter << YAML::Key << "Filepath" << YAML::Value << texture_observer.GetFilepath().string<PMR_STRING_TEMPLATE_PARAMS>(&pmr_p);
						emitter << YAML::Key << "UUID" << YAML::Value << texture_observer.GetUUID();
						emitter << YAML::EndMap;
					}
				}
				if (is_internal)
				{
					AssetObserver<Texture2D>(core.TextureIDs[i]).SaveMetadata(emitter);
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

	static void LoadUniforms(const YAML::Node& node, void* uniformsData, const Description::Buffer::Layout& uniforms)
	{
		FE_PROFILER_FUNC();

		char* uniform_data_ptr = (char*)uniformsData;

		for (const auto& uniform : uniforms.Elements)
		{
			const auto size = uniform.Size();
			const auto& count = uniform.Count;
			const auto& type = uniform.Type;
			const auto& name = uniform.Name;

			const auto& uniform_node = node[name.CData()];

			if (!uniform_node.IsDefined())
			{
				FE_LOG_CORE_WARN("Missing uniform in material definition");
				uniform_data_ptr += size * count;
				continue;
			}
			
			const auto& type_node = uniform_node["Type"];
			const auto& count_node = uniform_node["Count"];
			const auto& value_node = uniform_node["Value"];

			if (!type_node ||
				!count_node ||
				!value_node)
			{
				FE_LOG_CORE_WARN("Ill defined uniform in material definition");
				uniform_data_ptr += size * count;

				continue;
			}

			const auto uniform_count = count_node.as<uint32_t>();

			if (type.ToConstCharPtr() != type_node.as<std::string>() ||
				!value_node.IsSequence() ||
				value_node.size() != uniform_count ||
				count != uniform_count)
			{
				FE_LOG_CORE_WARN("Ill defined uniform '{0}' in material definition", name.CData());
				uniform_data_ptr += size * count;

				continue;
			}

			for (size_t i = 0; i < count; ++i)
			{
				bool success = LoadGPUDataType(value_node[i], uniform_data_ptr, type);
				if (!success) FE_LOG_CORE_WARN("Ill defined uniform '{0}' in material definition", name.CData());
				uniform_data_ptr += size;
			}
		}
	}

	static void LoadTextures(const YAML::Node& node, std::vector<AssetID>& textureIDs, const std::pmr::vector<uint32_t>& textureSamplerIDs, const std::filesystem::path& parentPath, AssetID master)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::Get().m_Registry;
		const auto& library = Description::Library::Get();

		for (size_t i = 0; i < textureSamplerIDs.size(); ++i)
		{

			const auto& texture_sampler = library.TextureSamplers[textureSamplerIDs[i]];
			const auto& texture_node = node[texture_sampler.Name.CData()];

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
			spec.ArchetypeID = texture_sampler.TextureArchetypeID;
			//spec.Components.FromString(components_node.as<std::string>());
			//spec.Format.FromString(format_node.as<std::string>());
			spec.Width = width_node.as<uint32_t>();
			spec.Height = height_node.as<uint32_t>();

			AssetManager::SetSourcePath(texture_id, parentPath / texture_source_filepath_node.as<std::string>());

			textureIDs[i] = texture_id;
		}
	}

	bool Material::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::Get().m_Registry;

		const auto& filepath = reg.get<ACFilepath>(assetID).Filepath;
		auto full_filepath = Project::Get()->m_AssetsPath / filepath;

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
		auto& sm_core = sm_observer.GetCore();
		auto& core = reg.get<Material::Core>(assetID);

		auto& library = Description::Library::Get();
		auto& program_spec = library.ProgramSpecs[sm_core.ProgramSpecificationID];

		auto& uniforms = library.BufferLayouts[program_spec.MainUniformsLayoutID];

		LoadUniforms(uniforms_node, core.UniformsData.Elements, uniforms);

		auto& texture_samplers = program_spec.TextureSamplerIDs;
		auto source_filepath = filepath.parent_path();

		FE_CORE_ASSERT(false, "Not implemented");
		//LoadTextures(textures_node, core.TextureIDs, texture_samplers, source_filepath, assetID);

		return true;
	}

	AssetID Material::LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::Get().m_Registry;

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
		const auto& sm_core = sm_observer.GetCore();

		const auto& uniforms_node = node["Uniforms"];
		const auto& textures_node = node["Textures"];

		if (!uniforms_node ||
			!textures_node)
		{
			FE_LOG_CORE_ERROR("Ill defined material");
			return NullAssetID;
		}

		auto& library = Description::Library::Get();
		auto& program_spec = library.ProgramSpecs[sm_core.ProgramSpecificationID];

		auto& uniforms = library.BufferLayouts[program_spec.MainUniformsLayoutID];
		
		
		LoadUniforms(uniforms_node, core.UniformsData.Elements, uniforms);
		auto& texture_samplers = program_spec.TextureSamplerIDs;
		FE_CORE_ASSERT(false, "not implemented");
		//LoadTextures(textures_node, core.TextureIDs, texture_samplers, parentPath, master);

		return asset_id;
	}

	void Material::MakeMaterial(AssetID assetID, const AssetObserver<ShadingModel>& shadingModelObserver)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::Get().m_Registry;

		auto& core_component = reg.get<Material::Core>(assetID);
		auto& sm_core_component = shadingModelObserver.GetCore();

		core_component.ShadingModelID = shadingModelObserver.GetID();

		auto& data = core_component.UniformsData.Elements;
		auto& size = core_component.UniformsData.Count;

		size = sm_core_component.UniformsDataSize;

		if (data)
			operator delete(data);
		data = (Byte*)operator new(size);

		std::memcpy(data, sm_core_component.DefaultUniformsData, size);

		auto& library = Description::Library::Get();
		auto& program_spec = library.ProgramSpecs[sm_core_component.ProgramSpecificationID];
		
		auto& textures = core_component.TextureIDs;
		FE_CORE_ASSERT(false, "Not implemented");
		//textures.clear();
		//textures.resize(program_spec.TextureSamplerIDs.Count);
		for (auto& texture : textures)
		{
			texture = NullAssetID;
		}
	}
}