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
		ShadingModelHandle.SetLoadingPriority(AssetLoadingPriority::None);
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

		core_component.ShadingModelHandle.SetID(shadingModelObserver.GetID());

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
			texture.SetLoadingPriority(AssetLoadingPriority::None);
		}
	}

	void* MaterialObserver::GetUniformValuePtr_Internal(const ACMaterialCore& dataComponent, const Uniform& targetUniform) const
	{
		FE_PROFILER_FUNC();

		uint8_t* uniformDataPointer = (uint8_t*)(dataComponent.UniformsData);

		auto shading_model_observer = dataComponent.ShadingModelHandle.Observe();
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

		auto shading_model_observer = dataComponent.ShadingModelHandle.Observe();

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

		auto shading_model_observer = dataComponent.ShadingModelHandle.Observe();

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

		auto shading_model_observer = dataComponent.ShadingModelHandle.Observe();

		auto slotsIt = shading_model_observer.GetCoreComponent().TextureSlots.begin();
		for (const auto& texture : dataComponent.Textures)
		{
			if (slotsIt._Ptr == &textureSlot)
				return texture.GetID();

			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return NullAssetID;
	}

	AssetID MaterialObserver::GetTextureID(const ACMaterialCore& dataComponent, const std::string& textureSlotName) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = dataComponent.ShadingModelHandle.Observe();

		auto slotsIt = shading_model_observer.GetCoreComponent().TextureSlots.begin();
		for (const auto& texture : dataComponent.Textures)
		{
			if (slotsIt->GetName() == textureSlotName)
				return texture.GetID();

			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
		return NullAssetID;
	}

	void MaterialUser::SetTexture(ACMaterialCore& dataComponent, const ShaderTextureSlot& textureSlot, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = dataComponent.ShadingModelHandle.Observe();

		auto slotsIt = shading_model_observer.GetCoreComponent().TextureSlots.begin();
		for (auto& texture : dataComponent.Textures)
		{
			if (slotsIt._Ptr == &textureSlot)
			{
				texture.SetID(textureID);
				return;
			}
			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::SetTexture(ACMaterialCore& dataComponent, const std::string& textureSlotName, AssetID textureID) const
	{
		FE_PROFILER_FUNC();

		auto shading_model_observer = dataComponent.ShadingModelHandle.Observe();

		auto slotsIt = shading_model_observer.GetCoreComponent().TextureSlots.begin();
		for (auto& texture : dataComponent.Textures)
		{
			if (slotsIt->GetName() == textureSlotName)
			{
				texture.SetID(textureID);
				return;
			}
			++slotsIt;
		}

		FE_CORE_ASSERT(false, "Texture not found in material!");
	}

	void MaterialUser::ResetUniformValueToDefault(ACMaterialCore& dataComponent, const Uniform& uniform) const
	{
		void* dest = GetUniformValuePtr_Internal(dataComponent, uniform);
		auto shading_model_observer = dataComponent.ShadingModelHandle.Observe();

		auto offset = (std::byte*)dest - (std::byte*)dataComponent.UniformsData;
		void* src = (std::byte*)shading_model_observer.GetCoreComponent().DefaultUniformsData + offset;

		std::memcpy(dest, src, uniform.GetSize());
	}

	bool MaterialUser::SendDataToGPU(GDIType GDI) const
	{
		auto& core = Get<ACMaterialCore>();
		
		if (!core.ShadingModelHandle.IsValid())
			return false;

		for (const auto& texture_handle : core.Textures)
		{
			if (!texture_handle.IsValid())
				return false;

			auto texture_user = texture_handle.Use();
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

		for (const auto& texture_handle : core.Textures)
		{
			if (!texture_handle.IsValid())
				continue;

			auto texture_user = texture_handle.Use();
			texture_user.ReleaseDependencyLoad();
		}
	}

	void Material::SaveMetadata(AssetID assetID)
	{
		auto assetObserver = AssetObserver<Material>(assetID);
		auto& core = assetObserver.GetCoreComponent();

		YAML::Emitter emitter;

		const auto shading_model_observer = core.ShadingModelHandle.Observe();
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
			if (core.Textures[i].IsValid())
			{
				const auto texture_observer = core.Textures[i].Observe();
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

		std::ofstream fout(Project::GetInstance()->AssetsPath / assetObserver.GetFilepath());
		fout << emitter.c_str();
	}

	bool Material::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		auto& core = ECS_handle.get<ACMaterialCore>();

		auto filepath = Project::GetInstance()->AssetsPath;
		filepath /= ECS_handle.get<ACFilepath>().Filepath;
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
		core.ShadingModelHandle.SetID(AssetManager::GetOrCreateAssetWithUUID(shading_model_UUID));

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
				core.Textures.emplace_back();			
			else
				core.Textures.emplace_back(AssetManager::GetOrCreateAssetWithUUID(texture_UUID_node.as<UUID>()));
		}

		return true;
	}
}