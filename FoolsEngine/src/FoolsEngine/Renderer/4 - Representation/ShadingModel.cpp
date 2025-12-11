#include "FE_pch.h"
#include "ShadingModel.h"

#include "FoolsEngine\Renderer\1 - Description\Library.h"
#include "ShaderAsset.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Assets\Serialization\GPUDataSerialization.h"

namespace fe
{
	void ACShadingModelCore::Init()
	{
		if (DefaultUniformsData) operator delete(DefaultUniformsData);
		DefaultUniformsData = nullptr;

		UniformsDataSize = 0;
		VertexShaderID = NullAssetID;
		FragmentShaderID = NullAssetID;
		ProgramSpecificationID = -1;
	}

	void* ShadingModelObserver::GetUniformDefaultValuePtr_Internal(const ACShadingModelCore& dataComponent, const Description::Buffer::Element& targetUniform) const
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

	void* ShadingModelObserver::GetUniformDefaultValuePtr_Internal(const ACShadingModelCore& dataComponent, const std::string& name) const
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

	void ShadingModelUser::SetUniformDefaultValue(const ACShadingModelCore& dataComponent, const Description::Buffer::Element& targetUniform, void* dataPointer) const
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

	void ShadingModelUser::SetUniformDefaultValue(const ACShadingModelCore& dataComponent, const std::string& name, void* dataPointer) const
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
	
	void ShadingModelObserver::SaveMetadata(YAML::Emitter& emitter)
	{
		FE_PROFILER_FUNC();

		auto& core = GetCoreComponent();
		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[core.ProgramSpecificationID];

		emitter << YAML::Key << "Program Specification" << YAML::Value << program_spec.UUID;
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

		emitter << YAML::Key << "Vertex Shader" << YAML::Value;
		{
			AssetObserver<ShaderAsset> vertex_observer(core.VertexShaderID);
			bool is_internal = vertex_observer.AllOf<ACMasterAsset>();

			if (!is_internal)
				vertex_observer.SaveMetadata(emitter);
			else
				emitter << vertex_observer.GetUUID();
		}
		emitter << YAML::Key << "Fragment Shader" << YAML::Value;
		{
			AssetObserver<ShaderAsset> fragment_observer(core.FragmentShaderID);
			bool is_internal = fragment_observer.AllOf<ACMasterAsset>();

			if (!is_internal)
				fragment_observer.SaveMetadata(emitter);
			else
				emitter << fragment_observer.GetUUID();
		}
	}

	bool ShadingModelUser::LoadMetadata()
	{
		FE_PROFILER_FUNC();

		
		const auto& filepath = GetFilepath();

		return DeserializeFromFile(filepath);
	}

	static bool LoadUniforms(const YAML::Node& node, void* uniformsData)
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

	bool ShadingModelUser::DeserializeFromFile(const std::filesystem::path& filepath)
	{
		FE_PROFILER_FUNC();

		auto& core = GetCoreComponent();

		YAML::Node node;
		
		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			node = YAML::LoadFile(filepath.string());
		}

		auto uuid_node = node["UUID"];
		if (uuid_node) // Base Assets don't have UUID in their file
		{
			if (GetUUID() != node["UUID"].as<UUID>())
			{
				FE_CORE_ASSERT(false, "Not machting UUID in asset and its metafile!");
				return false;
			}
		}
		else
		{
			FE_LOG_CORE_WARN("Missing UUID in ShadingModel file");
		}

		const auto& program_spec_node	= node["Program Specification"];
		const auto& data_size_node		= node["Uniforms Data Size"];
		const auto& uniforms_node		= node["Uniforms"];
		const auto& vertex_node			= node["Vertex Shader"];
		const auto& fragmnent_node		= node["Fragment Shader"];

		if (!program_spec_node ||
			!data_size_node ||
			!uniforms_node ||
			!vertex_node ||
			!fragmnent_node)
		{
			FE_LOG_CORE_ERROR("Ill specified ShadingModel");
			return false;
		}

		auto& library = Description::Library::Get();
		UUID program_uuid = program_spec_node.as<UUID>();
		uint32_t program_spec_id = library.CreateOrGetDescriptorWithUUID<Description::ShaderInterface::ProgramSpecification>(program_uuid);
		core.ProgramSpecificationID = program_spec_id;

		core.UniformsDataSize = data_size_node.as<size_t>();
		core.DefaultUniformsData = operator new(core.UniformsDataSize);

		bool success = LoadUniforms(uniforms_node, core.DefaultUniformsData);
		if (!success)
		{
			FE_LOG_CORE_ERROR("Ill specified uniforms in ShadingModel");
			return false;
		}

		return true;
	}
}