#include "FE_pch.h"
#include "ShadingModel.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"

#include "FoolsEngine\Assets\Serializers\YAML.h"

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

	template <typename data_type>
	void EmitVector(YAML::Emitter& emitter, char* uniform_data_ptr, ShaderData::Type type)
	{
		emitter << YAML::Flow << YAML::BeginSeq;
		for (size_t i = 0; i < ShaderData::CountInVector(type); ++i)
		{
			emitter << (data_type)*uniform_data_ptr;
			uniform_data_ptr += ShaderData::SizeOfPrimitive(primitive);
		}
		emitter << YAML::EndSeq;
	}

	template <typename data_type>
	void EmitMatrix(YAML::Emitter& emitter, char* uniform_data_ptr, ShaderData::Type type)
	{
		emitter << YAML::Flow << YAML::BeginSeq;
		auto primitive_size = ShaderData::SizeOfPrimitive(primitive);
		for (size_t i = 0; i < ShaderData::RowsOfMatrix(type); ++i)
		{
			emitter << YAML::Flow << YAML::BeginSeq;
			for (size_t j = 0; j < ShaderData::ColumnsOfMatrix(type); ++j)
			{
				emitter << (data_type)*uniform_data_ptr;
				uniform_data_ptr += primitive_size;
			}
			emitter << YAML::EndSeq;
		}
		emitter << YAML::EndSeq;
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

			auto& type = uniform.GetType();
			auto structure = uniform.GetStructure();
			auto primitive = uniform.GetPrimitive();
			for (size_t i = 0; i < uniform.GetCount(); i++)
			{
				switch (structure)
				{
				case ShaderData::Structure::Scalar:
					switch (primitive)
					{
					case ShaderData::Primitive::Bool:   emitter << (bool    )*uniform_data_ptr; break;
					case ShaderData::Primitive::Int:    emitter << (int32_t )*uniform_data_ptr; break;
					case ShaderData::Primitive::UInt:   emitter << (uint32_t)*uniform_data_ptr; break;
					case ShaderData::Primitive::Float:  emitter << (float   )*uniform_data_ptr; break;
					case ShaderData::Primitive::Double: emitter << (double  )*uniform_data_ptr; break;
					default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
					}
					break;
				case ShaderData::Structure::Vector:
					switch (primitive)
					{
					case ShaderData::Primitive::Bool:   EmitVector<bool    >(emitter, uniform_data_ptr, type); break;
					case ShaderData::Primitive::Int:    EmitVector<int     >(emitter, uniform_data_ptr, type); break;
					case ShaderData::Primitive::UInt:   EmitVector<uint32_t>(emitter, uniform_data_ptr, type); break;
					case ShaderData::Primitive::Float:  EmitVector<float   >(emitter, uniform_data_ptr, type); break;
					case ShaderData::Primitive::Double: EmitVector<double  >(emitter, uniform_data_ptr, type); break;
					default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
					}
					break;
				case ShaderData::Structure::Matrix:
					switch (primitive)
					{
					case ShaderData::Primitive::Bool:   EmitVector<bool    >(emitter, uniform_data_ptr, type); break;
					case ShaderData::Primitive::Int:    EmitVector<int     >(emitter, uniform_data_ptr, type); break;
					case ShaderData::Primitive::UInt:   EmitVector<uint32_t>(emitter, uniform_data_ptr, type); break;
					case ShaderData::Primitive::Float:  EmitVector<float   >(emitter, uniform_data_ptr, type); break;
					case ShaderData::Primitive::Double: EmitVector<double  >(emitter, uniform_data_ptr, type); break;
					default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
					}
				default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Structure");
				}

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

	template <typename data_type>
	bool LoadVector(const YAML::Node& node, char* uniform_data_ptr, ShaderData::Type type)
	{
		size_t vector_size = ShaderData::CountInVector(type);
		size_t primitive_size = ShaderData::SizeOfPrimitive(ShaderData::PrimitiveInType(type));

		if (!node.IsSequence() || node.size() != vector_size) return false; 
		for (size_t i = 0; i < vector_size; ++i)
		{
			*(data_type*)uniform_data_ptr = node[i].as<data_type>();
			uniform_data_ptr += primitive_size;
		}
		return true;
	}

	template <typename data_type>
	bool LoadMatrix(const YAML::Node& node, char* uniform_data_ptr, ShaderData::Type type)
	{
		size_t primitive_size = ShaderData::SizeOfPrimitive(ShaderData::PrimitiveInType(type));
		size_t rows = ShaderData::RowsOfMatrix(type);
		size_t columns = ShaderData::ColumnsOfMatrix(type);

		if (!node.IsSequence() || node.size() != rows) return false;
		for (size_t i = 0; i < rows; ++i)
		{
			const auto& row_node = node[i];
			if (!row_node.IsSequence() || row_node.size() != columns) return false;
			
			for (size_t j = 0; j < columns; ++j)
			{
				*(data_type*)uniform_data_ptr = row_node[j].as<data_type>();
				uniform_data_ptr += primitive_size;
			}
		}
		return true;
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
			auto primitive = ShaderData::PrimitiveInType(uniform_type);
			for (size_t i = 0; i < uniform_count; ++i)
			{
				const auto& uniform_instance_node = value_node[i];
				auto structure = ShaderData::StructureInType(uniform_type);
				bool success;

				switch (structure)
				{
				case ShaderData::Structure::Scalar:
					switch (primitive)
					{
					case ShaderData::Primitive::Bool:   *(bool*    )uniform_data_ptr = uniform_instance_node.as<bool    >(); break;
					case ShaderData::Primitive::Int:    *(int*     )uniform_data_ptr = uniform_instance_node.as<int     >(); break;
					case ShaderData::Primitive::UInt:   *(uint32_t*)uniform_data_ptr = uniform_instance_node.as<uint32_t>(); break;
					case ShaderData::Primitive::Float:  *(float*   )uniform_data_ptr = uniform_instance_node.as<float   >(); break;
					case ShaderData::Primitive::Double: *(double*  )uniform_data_ptr = uniform_instance_node.as<double  >(); break;
					default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
					}
					break;
				case ShaderData::Structure::Vector:
					switch (primitive)
					{
					case ShaderData::Primitive::Bool:   success = LoadVector<bool    >(uniform_instance_node, uniform_data_ptr, uniform_type); break;
					case ShaderData::Primitive::Int:    success = LoadVector<int     >(uniform_instance_node, uniform_data_ptr, uniform_type); break;
					case ShaderData::Primitive::UInt:   success = LoadVector<uint32_t>(uniform_instance_node, uniform_data_ptr, uniform_type); break;
					case ShaderData::Primitive::Float:  success = LoadVector<float   >(uniform_instance_node, uniform_data_ptr, uniform_type); break;
					case ShaderData::Primitive::Double: success = LoadVector<double  >(uniform_instance_node, uniform_data_ptr, uniform_type); break;
					default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
					}
					break;
				case ShaderData::Structure::Matrix:
					switch (primitive)
					{
					case ShaderData::Primitive::Bool:   success = LoadMatrix<bool    >(uniform_instance_node, uniform_data_ptr, uniform_type); break;
					case ShaderData::Primitive::Int:    success = LoadMatrix<int     >(uniform_instance_node, uniform_data_ptr, uniform_type); break;
					case ShaderData::Primitive::UInt:   success = LoadMatrix<uint32_t>(uniform_instance_node, uniform_data_ptr, uniform_type); break;
					case ShaderData::Primitive::Float:  success = LoadMatrix<float   >(uniform_instance_node, uniform_data_ptr, uniform_type); break;
					case ShaderData::Primitive::Double: success = LoadMatrix<double  >(uniform_instance_node, uniform_data_ptr, uniform_type); break;
					default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Primitive");
					}
				default: FE_CORE_ASSERT(false, "Unrecognized ShaderData::Structure");
				}
				if (!success) return false;

				uniform_data_ptr += ShaderData::SizeOfType(uniform_type);
			}
		}

		for (const auto& texture_slot_node : texture_slots_node)
		{
			core.TextureSlots.emplace_back(texture_slot_node.as<std::string>(), TextureData::Type::Texture2D);
		}

		return true;
	}
}