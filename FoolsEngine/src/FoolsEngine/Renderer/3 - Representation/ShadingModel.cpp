#include "FE_pch.h"
#include "ShadingModel.h"

#include "FoolsEngine\Renderer\1 - Description\Uniform.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Assets\Serialization\GPUDataSerialization.h"

namespace fe
{
	void ACShadingModelCore::Init()
	{
		Uniforms.clear();
		TextureSlots.clear();

		if (DefaultUniformsData) operator delete(DefaultUniformsData);
		DefaultUniformsData = nullptr;

		UniformsDataSize = 0;
		ShaderID = NullAssetID;
	}

	void* ShadingModelObserver::GetUniformDefaultValuePtr_Internal(const ACShadingModelCore& dataComponent, const Uniform& targetUniform) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniform_data_pointer = (uint8_t*)(dataComponent.DefaultUniformsData);

		for (const auto& uniform : dataComponent.Uniforms)
		{
			if (&targetUniform == &uniform)
			{
				return (void*)uniform_data_pointer;
			}
			uniform_data_pointer += uniform.GetSize();
		}

		FE_CORE_ASSERT(false, "Uniform not found in ShadingModel!");
		return nullptr;
	}

	void* ShadingModelObserver::GetUniformDefaultValuePtr_Internal(const ACShadingModelCore& dataComponent, const std::string& name) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniform_data_pointer = (uint8_t*)(dataComponent.DefaultUniformsData);

		for (const auto& uniform : dataComponent.Uniforms)
		{
			if (name == uniform.m_Name)
			{
				return (void*)uniform_data_pointer;
			}
			uniform_data_pointer += uniform.GetSize();
		}

		FE_CORE_ASSERT(false, "Uniform not found in ShadingModel!");
		return nullptr;
	}

	void ShadingModelUser::SetUniformDefaultValue(const ACShadingModelCore& dataComponent, const Uniform& targetUniform, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		if (!dataPointer)
		{
			FE_CORE_ASSERT(false, "Pointer is null!");
			return;
		}

		void* dest = GetUniformDefaultValuePtr_Internal(dataComponent, targetUniform);
		std::memcpy((void*)dest, dataPointer, targetUniform.GetSize());
	}

	void ShadingModelUser::SetUniformDefaultValue(const ACShadingModelCore& dataComponent, const std::string& name, void* dataPointer) const
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
			if (name == uniform.m_Name)
			{
				std::memcpy((void*)dest, dataPointer, size);
				return;
			}
			dest += size;
		}

		FE_CORE_ASSERT(false, "Uniform not found in ShadingModel!");
	}
	
	void ShadingModel::SaveMetadata(YAML::Emitter& emitter, AssetID assetID)
	{
		FE_PROFILER_FUNC();

		auto asset_observer = AssetObserver<ShadingModel>(assetID);
		auto& core = asset_observer.GetCoreComponent();

		emitter << YAML::Key << "Shader" << YAML::Value << AssetManager::Get().m_Registry.get<ACFilepath>(core.ShaderID).Filepath.string();
		emitter << YAML::Key << "Uniforms Data Size" << YAML::Value << core.UniformsDataSize;
		emitter << YAML::Key << "Uniforms" << YAML::Value << YAML::BeginSeq;
		
		char* uniform_data_ptr = (char*)core.DefaultUniformsData;
		for (auto& uniform : core.Uniforms)
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << uniform.m_Name;
			emitter << YAML::Key << "Type" << YAML::Value << uniform.m_Type.ToConstCharPtr();
			emitter << YAML::Key << "Count" << YAML::Value << uniform.m_Count;
			emitter << YAML::Key << "Default Value" << YAML::Value << YAML::BeginSeq;

			for (size_t i = 0; i < uniform.m_Count; i++)
			{
				EmitGPUDataType(emitter, uniform_data_ptr, uniform.m_Type);
				uniform_data_ptr += uniform.GetSize();
			}
			emitter << YAML::EndSeq;
		}
		emitter << YAML::EndSeq;

		emitter << YAML::Key << "Shader Texture Slots" << YAML::Value << YAML::BeginSeq;
		for (size_t i = 0; i < core.TextureSlots.size(); ++i)
		{
			emitter << core.TextureSlots[i].m_Name;
		}
		emitter << YAML::EndSeq;

		//std::ofstream fout(asset_observer.GetFilepath());
		//fout << emitter.c_str();
	}

	bool ShadingModel::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		const auto& filepath = ECS_AssetHandle(AssetManager::Get().m_Registry, assetID).get<ACFilepath>().Filepath;

		return DeserializeFromFile(assetID, filepath);
	}

	static bool LoadUniforms(const YAML::Node& node, void* uniformsData, std::vector<Uniform>& uniforms)
	{
		FE_PROFILER_FUNC();

		char* uniform_data_ptr = (char*)uniformsData;

		for (auto& uniform_node : node)
		{
			FE_PROFILER_SCOPE("Uniform");

			const auto& name_node = uniform_node["Name"];
			const auto& type_node = uniform_node["Type"];
			const auto& count_node = uniform_node["Count"];
			const auto& value_node = uniform_node["DefaultValue"];

			if (!name_node) return false;
			if (!type_node) return false;
			if (!count_node) return false;
			if (!value_node) return false;

			const auto uniform_name = name_node.as<std::string>();
			Description::Data::Type uniform_type; uniform_type.FromString(type_node.as<std::string>());
			const auto uniform_count = count_node.as<uint32_t>();

			uniforms.emplace_back(uniform_name, uniform_type, uniform_count);

			if (!value_node.IsSequence() || value_node.size() != uniform_count) return false;
			auto uniform_size = Description::Data::SizeOfType(uniform_type);
			for (size_t i = 0; i < uniform_count; ++i)
			{
				bool success = LoadGPUDataType(value_node[i], uniform_data_ptr, uniform_type);
				if (!success) return false;

				uniform_data_ptr += uniform_size;
			}
		}

		return true;
	}

	bool ShadingModel::DeserializeFromFile(AssetID assetID, const std::filesystem::path& filepath)
	{
		FE_PROFILER_FUNC();

		ECS_AssetHandle ECS_handle(AssetManager::Get().m_Registry, assetID);

		auto& core = ECS_handle.get<ACShadingModelCore>();

		YAML::Node node;
		
		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			node = YAML::LoadFile(filepath.string());
		}

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
			FE_LOG_CORE_WARN("Missing UUID in ShadingModel file");
		}

		const auto& shader_node        = node["Shader"];
		const auto& data_size_node     = node["Uniforms Data Size"];
		const auto& uniforms_node      = node["Uniforms"];
		const auto& texture_slots_node = node["Shader Texture Slots"];

		if (!shader_node ||
			!data_size_node ||
			!uniforms_node ||
			!texture_slots_node ||
			!texture_slots_node.IsSequence())
		{
			FE_LOG_CORE_ERROR("Ill specified ShadingModel");
			return false;
		}

		std::filesystem::path shader_filepath = shader_node.as<std::string>();
		auto shaderID = AssetManager::GetAssetFromFilepath(shader_filepath);
		FE_CORE_ASSERT(shaderID != NullAssetID, "Failed to recognize shader asset during shading model deserialization");
		core.ShaderID = shaderID;

		core.UniformsDataSize = data_size_node.as<size_t>();
		core.DefaultUniformsData = operator new(core.UniformsDataSize);

		bool success = LoadUniforms(uniforms_node, core.DefaultUniformsData, core.Uniforms);
		if (!success)
		{
			FE_LOG_CORE_ERROR("Ill specified uniforms in ShadingModel");
			return false;
		}

		{
			FE_PROFILER_SCOPE("Texture Slots");
			for (const auto& texture_slot_node : texture_slots_node)
			{
				core.TextureSlots.emplace_back(texture_slot_node.as<std::string>(), Description::Texture::Type::Texture2D);
			}
		}

		return true;
	}
}