#include "FE_pch.h"
#include "Material.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"

#include "FoolsEngine\Assets\Serializers\YAML.h"

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
					case ShaderData::Primitive::Bool:   emitter << (bool)*uniform_data_ptr; break;
					case ShaderData::Primitive::Int:    emitter << (int32_t)*uniform_data_ptr; break;
					case ShaderData::Primitive::UInt:   emitter << (uint32_t)*uniform_data_ptr; break;
					case ShaderData::Primitive::Float:  emitter << (float)*uniform_data_ptr; break;
					case ShaderData::Primitive::Double: emitter << (double)*uniform_data_ptr; break;
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

	bool Material::Deserialize(AssetID assetID)
	{
		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		const auto& filepath = ECS_handle.get<ACFilepath>().Filepath;
		auto& core = ECS_handle.get<ACMaterialCore>();

		YAML::Node node = YAML::LoadFile(filepath.string());

		const auto& shading_model_node = node["Shading Model"];
		const auto& data_size_node = node["Uniforms Data Size"];
		const auto& uniforms_node = node["Uniforms"];
		const auto& textures_node = node["Textures"];

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
			const auto& name_node = uniform_node["Name"];
			const auto& type_node = uniform_node["Type"];
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
					case ShaderData::Primitive::Bool:   *(bool*)uniform_data_ptr = uniform_instance_node.as<bool    >(); break;
					case ShaderData::Primitive::Int:    *(int*)uniform_data_ptr = uniform_instance_node.as<int     >(); break;
					case ShaderData::Primitive::UInt:   *(uint32_t*)uniform_data_ptr = uniform_instance_node.as<uint32_t>(); break;
					case ShaderData::Primitive::Float:  *(float*)uniform_data_ptr = uniform_instance_node.as<float   >(); break;
					case ShaderData::Primitive::Double: *(double*)uniform_data_ptr = uniform_instance_node.as<double  >(); break;
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