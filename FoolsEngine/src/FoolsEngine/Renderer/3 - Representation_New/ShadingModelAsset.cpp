#include "FE_pch.h"
#include "ShadingModelAsset.h"


#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Assets\Serialization\GPUDataSerialization.h"

namespace fe
{
	void ACShadingModelAssetCore::Init()
	{
		if (DefaultUniformsData) operator delete(DefaultUniformsData);
		DefaultUniformsData = nullptr;

		UniformsDataSize = 0;
		VertexShaderID = NullAssetID;
		FragmentShaderID = NullAssetID;
	}

	void* ShadingModelAssetObserver::GetUniformDefaultValuePtr_Internal(const ACShadingModelAssetCore& dataComponent, const Description::Buffer::Element& targetUniform) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniform_data_pointer = (uint8_t*)(dataComponent.DefaultUniformsData);

		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[dataComponent.ProgramSpecificationID];
		const auto& uniforms_layout = library.BufferLayouts[program_spec.MainUniformsLayoutID];

		for (const auto& uniform : uniforms_layout.Elements)
		{
			if (&targetUniform == &uniform)
			{
				return (void*)uniform_data_pointer;
			}
			uniform_data_pointer += uniform.Size() * uniform.Count;
		}

		FE_CORE_ASSERT(false, "Uniform not found in ShadingModel!");
		return nullptr;
	}

	void* ShadingModelAssetObserver::GetUniformDefaultValuePtr_Internal(const ACShadingModelAssetCore& dataComponent, const std::string& name) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniform_data_pointer = (uint8_t*)(dataComponent.DefaultUniformsData);

		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[dataComponent.ProgramSpecificationID];
		const auto& uniforms_layout = library.BufferLayouts[program_spec.MainUniformsLayoutID];

		for (const auto& uniform : uniforms_layout.Elements)
		{
			if (name.compare(uniform.Name))
			{
				return (void*)uniform_data_pointer;
			}
			uniform_data_pointer += uniform.Size() * uniform.Count;
		}

		FE_CORE_ASSERT(false, "Uniform not found in ShadingModel!");
		return nullptr;
	}

	void ShadingModelAssetUser::SetUniformDefaultValue(const ACShadingModelAssetCore& dataComponent, const Description::Buffer::Element& targetUniform, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		if (!dataPointer)
		{
			FE_CORE_ASSERT(false, "Pointer is null!");
			return;
		}

		void* dest = GetUniformDefaultValuePtr_Internal(dataComponent, targetUniform);
		std::memcpy((void*)dest, dataPointer, targetUniform.Size() *  targetUniform.Count);
	}

	void ShadingModelAssetUser::SetUniformDefaultValue(const ACShadingModelAssetCore& dataComponent, const std::string& name, void* dataPointer) const
	{
		FE_PROFILER_FUNC();

		if (!dataPointer)
		{
			FE_CORE_ASSERT(false, "Pointer is null!");
			return;
		}

		uint8_t* dest = (uint8_t*)(dataComponent.DefaultUniformsData);

		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[dataComponent.ProgramSpecificationID];
		const auto& uniforms_layout = library.BufferLayouts[program_spec.MainUniformsLayoutID];

		for (const auto& uniform : uniforms_layout.Elements)
		{
			if (name.compare(uniform.Name))
			{
				std::memcpy((void*)dest, dataPointer, uniform.Size() * uniform.Count);
				return;
			}
			dest += uniform.Size() * uniform.Count;
		}

		FE_CORE_ASSERT(false, "Uniform not found in ShadingModel!");
	}
	
	void ShadingModelAssetObserver::SaveMetadata(YAML::Emitter& emitter)
	{
		FE_PROFILER_FUNC();

		auto& core = GetCoreComponent();
		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[core.ProgramSpecificationID];

		emitter << YAML::Key << "Shader" << YAML::Value << GetFilepath().string();
		emitter << YAML::Key << "Uniforms Data Size" << YAML::Value << core.UniformsDataSize;
		emitter << YAML::Key << "Uniforms" << YAML::Value << YAML::BeginSeq;
		
		char* uniform_data_ptr = (char*)core.DefaultUniformsData;
		const auto& uniforms_layout = library.BufferLayouts[program_spec.MainUniformsLayoutID];
		for (auto& uniform : uniforms_layout.Elements)
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << uniform.Name;
			emitter << YAML::Key << "Type" << YAML::Value << uniform.Type.ToConstCharPtr();
			emitter << YAML::Key << "Count" << YAML::Value << uniform.Count;
			emitter << YAML::Key << "Default Value" << YAML::Value << YAML::BeginSeq;

			for (size_t i = 0; i < uniform.Count; i++)
			{
				EmitGPUDataType(emitter, uniform_data_ptr, uniform.Type);
				uniform_data_ptr += uniform.Size();
			}
			emitter << YAML::EndSeq << YAML::EndMap;
		}
		emitter << YAML::EndSeq;

		emitter << YAML::Key << "Shader Texture Slots" << YAML::Value << YAML::BeginSeq;
		for (size_t i = 0; i < program_spec.TextureSamplers.size(); ++i)
		{
			const auto& spec = program_spec.TextureSamplers[i].Spec;

			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name"					<< program_spec.TextureSamplers[i].Name;
			emitter << YAML::Key << "Type"					<< spec.Type.ToConstCharPtr();
			emitter << YAML::Key << "Format"				<< spec.Format.ToConstCharPtr();
			emitter << YAML::Key << "Wrapping"				<< spec.Wrapping.ToConstCharPtr();
			emitter << YAML::Key << "Filtering"				<< spec.Filtering.ToConstCharPtr();
			emitter << YAML::Key << "Mipmapping"			<< spec.Mipmapping.ToConstCharPtr();
			emitter << YAML::Key << "AnisotropicFiltering"	<< spec.AnisotropicFiltering.ToConstCharPtr();
			emitter << YAML::EndMap;

		}
		emitter << YAML::EndSeq;
	}

	bool ShadingModelAssetObserver::LoadMetadata()
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