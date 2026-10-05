#include "FE_pch.h"
#include "Texture.h"

#include "FoolsEngine/Foundation/Memory/Scratchpad.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetAccessors.h"
#include "FoolsEngine/Assets/Loaders/TextureLoader.h"
#include "FoolsEngine/Assets/Serialization/YAML.h"

#include "FoolsEngine/Renderer/7 - Integration/Renderer.h"

namespace fe::Render::Representation
{
	void Texture2DObserver::SaveMetadata(YAML::Emitter& emitter)
	{
		FE_PROFILER_FUNC();
		
		Scratchpad sp;
		auto& spec = GetCore().mSpecification;

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << GetUUID();
		emitter << YAML::Key << "Source Filepath" << YAML::Value << GetSourceFilepath()->Filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str();
		emitter << YAML::Key << "Usage" << YAML::Value << spec.Usage.ToConstCharPtr();
		emitter << YAML::Key << "Archetype" << YAML::Value << archetype.UUID;
		emitter << YAML::Key << "Width" << YAML::Value << spec.mDimentions;
		emitter << YAML::Key << "Height" << YAML::Value << spec.Height;
		emitter << YAML::EndMap;
	}

	bool Texture2DUser::LoadMetadata()
	{
		FE_PROFILER_FUNC();

		auto filepath = Project::Get()->m_AssetsPath;
		const auto& relative_path = Get<ACFilepath>().Filepath;
		filepath /= relative_path;
		
		YAML::Node node;

		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			node = YAML::LoadFile(filepath.string());
		}

		const auto& UUID_node = node["UUID"];

		if (!UUID_node.IsDefined())
		{
			FE_LOG_CORE_ERROR("Misspecified Texture in serialized node");
			return false;
		}

		auto uuid = UUID_node.as<UUID>();
		if (uuid == UUID(0))
		{
			FE_LOG_CORE_ERROR("Missing Texture definition!");
			return false;
		}

		if (!node["Usage"].IsDefined() ||
			!node["Archetype"].IsDefined() ||
			!node["Width"].IsDefined() ||
			!node["Height"].IsDefined() ||
			!node["Source Filepath"].IsDefined())
		{
			FE_LOG_CORE_ERROR("Ill defined Texture in serialized node");
			return false;
		}

		auto& spec = GetCore().Specification;
		
		spec.Usage.FromString(node["Usage"].as<std::string>());

		auto& lib = Description::Library::Get();
		auto spec_uuid = node["Archetype"].as<UUID>();
		spec.ArchetypeID = lib.CreateOrGetDescriptorWithUUID<Description::ShaderInterface::Specification>(spec_uuid);

		spec.Width = node["Width"].as<uint32_t>();
		spec.Height = node["Height"].as<uint32_t>();

		std::filesystem::path source_path = node["Source Filepath"].as<std::string>();
		AssetManager::SetSourcePath(GetID(), source_path);

		return true;
	}

	AssetID Texture2D::LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentpath)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::Get().m_Registry;

		const auto& UUID_node = node["UUID"];

		if (!UUID_node.IsDefined())
		{
			FE_LOG_CORE_ERROR("Misspecified Texture in serialized node");
			return NullAssetID;
		}

		auto uuid = UUID_node.as<UUID>();
		if (uuid == UUID(0))
		{
			FE_LOG_CORE_ERROR("Missing Texture definition!");
			return NullAssetID;
		}

		auto asset_id = AssetManager::GetOrCreateAssetWithUUID(uuid);
		if (reg.all_of<ACRefsCounters>(asset_id)) return asset_id; // is ProjectAsset ?

		if (!node["Usage"].IsDefined() ||
			!node["Archetype"].IsDefined() ||
			!node["Width"].IsDefined() ||
			!node["Height"].IsDefined() ||
			!node["Source Filepath"].IsDefined())
		{
			FE_LOG_CORE_ERROR("Ill defined Texture in serialized node");
			return NullAssetID;
		}

		reg.emplace<ACAssetType>(asset_id).Type = AssetType::Texture2D;
		reg.emplace<ACMasterAsset>(asset_id).Master = master;
		auto& core = reg.emplace<Texture2D::Core>(asset_id);
		core.Init();

		auto& spec = core.Specification;

		spec.Usage.FromString(node["Usage"].as<std::string>());

		auto& lib = Description::Library::Get();
		auto spec_uuid = node["Archetype"].as<UUID>();
		spec.ArchetypeID = lib.CreateOrGetDescriptorWithUUID<Description::ShaderInterface::Specification>(spec_uuid);
		
		spec.Width = node["Width"].as<uint32_t>();
		spec.Height = node["Height"].as<uint32_t>();

		std::filesystem::path source_path = parentpath;
		source_path /= node["Source Filepath"].as<std::string>();
		AssetManager::SetSourcePath(asset_id, source_path);

		return asset_id;
	}
}