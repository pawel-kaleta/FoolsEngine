#include "FE_pch.h"
#include "ShadingModel.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Assets\Serialization\ShaderDataSerialization.h"

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

	void* ShadingModelObserver::GetUniformDefaultValuePtr_Internal(const ACShadingModelCore& dataComponent, const Uniform& targetUniform) const
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

	void* ShadingModelObserver::GetUniformDefaultValuePtr_Internal(const ACShadingModelCore& dataComponent, const std::string& name) const
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
			if (name == uniform.GetName())
			{
				std::memcpy((void*)dest, dataPointer, size);
				return;
			}
			dest += size;
		}

		FE_CORE_ASSERT(false, "Uniform not found in ShadingModel!");
	}
	
	void ShadingModel::Serialize(const AssetObserver<ShadingModel>& assetObserver)
	{
		auto& core = assetObserver.GetCoreComponent();

		YAML::Emitter emitter;

		emitter << YAML::Key << "Shader" << YAML::Value << AssetManager::GetRegistry().get<ACFilepath>(core.ShaderID).Filepath.string();
		emitter << YAML::Key << "Uniforms Data Size" << YAML::Value << core.UniformsDataSize;
		emitter << YAML::Key << "Uniforms" << YAML::Value << YAML::BeginSeq;
		
		char* uniform_data_ptr = (char*)core.DefaultUniformsData;
		for (auto& uniform : core.Uniforms)
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << uniform.GetName();
			emitter << YAML::Key << "Type" << YAML::Value << uniform.GetType().ToString();
			emitter << YAML::Key << "Count" << YAML::Value << uniform.GetCount();
			emitter << YAML::Key << "Default Value" << YAML::Value << YAML::BeginSeq;

			auto type = uniform.GetType();
			for (size_t i = 0; i < uniform.GetCount(); i++)
			{
				EmitShaderDataType(emitter, uniform_data_ptr, type);
				uniform_data_ptr += uniform.GetSize();
			}
			emitter << YAML::EndSeq;
		}
		emitter << YAML::EndSeq;

		emitter << YAML::Key << "Shader Texture Slots" << YAML::Value << YAML::BeginSeq;
		for (size_t i = 0; i < core.TextureSlots.size(); ++i)
		{
			emitter << core.TextureSlots[i].GetName();
		}
		emitter << YAML::EndSeq;

		std::ofstream fout(assetObserver.GetFilepath());
		fout << emitter.c_str();
	}

	bool ShadingModel::Deserialize(AssetID assetID)
	{
		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		const auto& filepath = ECS_handle.get<ACFilepath>().Filepath;
		auto& core = ECS_handle.get<ACShadingModelCore>();

		YAML::Node node = YAML::LoadFile(filepath.string());

		const auto& shader_node        = node["Shader"];
		const auto& data_size_node     = node["Uniforms Data Size"];
		const auto& uniforms_node      = node["Uniforms"];
		const auto& texture_slots_node = node["Shader Texture Slots"];

		if (!shader_node) return false;
		if (!data_size_node) return false;
		if (!uniforms_node) return false;
		if (!texture_slots_node) return false;
		if (!texture_slots_node.IsSequence()) return false;

		std::filesystem::path shader_filepath = shader_node.as<std::string>();
		core.ShaderID = AssetManager::GetAssetFromFilepath(shader_filepath);

		core.UniformsDataSize = data_size_node.as<size_t>();
		core.DefaultUniformsData = operator new(core.UniformsDataSize);

		char* uniform_data_ptr = (char*)core.DefaultUniformsData;

		for (auto& uniform_node : uniforms_node)
		{
			const auto& name_node  = uniform_node["Name"];
			const auto& type_node  = uniform_node["Type"];
			const auto& count_node = uniform_node["Count"];
			const auto& value_node = uniform_node["Default Value"];

			if (!name_node) return false;
			if (!type_node) return false;
			if (!count_node) return false;
			if (!value_node) return false;

			const auto uniform_name = name_node.as<std::string>();
			ShaderData::Type uniform_type; uniform_type.FromInt(type_node.as<int>());
			const auto uniform_count = count_node.as<uint32_t>();

			core.Uniforms.emplace_back(uniform_name, uniform_type, uniform_count);

			if (!value_node.IsSequence() || value_node.size() != uniform_count) return false;
			auto uniform_size = ShaderData::SizeOfType(uniform_type);
			for (size_t i = 0; i < uniform_count; ++i)
			{
				bool success = LoadShaderDataType(value_node[i], uniform_data_ptr, uniform_type);
				if (!success) return false;

				uniform_data_ptr += uniform_size;
			}
		}

		for (const auto& texture_slot_node : texture_slots_node)
		{
			core.TextureSlots.emplace_back(texture_slot_node.as<std::string>(), TextureData::Type::Texture2D);
		}

		return true;
	}
}