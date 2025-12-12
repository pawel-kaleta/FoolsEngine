#include "FE_pch.h"
#include "Texture.h"

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"
#include "FoolsEngine\Renderer\7 - Integration\Renderer.h"

#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Assets\AssetAccessors.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Core\Project.h"

#include "FoolsEngine\Memory\Scratchpad.h"

namespace fe
{
	bool Texture2DUser::SendDataToGPU(GAPIType GAPI) const
	{
		FE_PROFILER_FUNC();

		auto& core = GetCoreComponent();

		if (!core.Data)
			return false;

		switch (GAPI.Value)
		{
		case GAPIType::None:
			FE_CORE_ASSERT(false, "Unspecified GAPIType");
			break;

		case GAPIType::OpenGL:
			auto& resource = CreateResourceComponent<GAPIType::OpenGL>().Texture;
			resource.Height = core.Height;
			resource.Width = core.Width;
			resource.SpecificationID = core.SpecificationID;
			resource.Usage = core.Usage;
			resource.Create(core.Data);
			break;
		}

		return true;
	}

	void Texture2DUser::UnloadFromCPU() const
	{
		FE_PROFILER_FUNC();

		auto& data_ptr = Get<ACTexture2DCore>().Data;
		if (data_ptr)
		{
			TextureLoader::UnloadTexture(data_ptr);
			data_ptr = nullptr;
		}
	}

	void Texture2DUser::Release() const
	{
		FE_PROFILER_FUNC();

		auto GAPI = Renderer::GetActiveGAPIType();
		switch (GAPI.Value)
		{
		case GAPIType::None:
			FE_CORE_ASSERT(false, "Unspecified GAPIType");
			break;

		case GAPIType::OpenGL:
			if (AllOf<ACTexture2DResource_OpenGL>())
			{
				Scratchpad sp;
				FE_LOG_CORE_DEBUG("Unloading Texture from GPU, AssetID: {0}, Name: {1}", GetID(), GetFilepath().string<PMR_STRING_TEMPLATE_PARAMS>(&sp));
				Get<ACTexture2DResource_OpenGL>().Texture.Destroy();
				Erase<ACTexture2DResource_OpenGL>();
			}
			break;
		}
	}

	void Texture2DObserver::SaveMetadata(YAML::Emitter& emitter)
	{
		FE_PROFILER_FUNC();
		
		Scratchpad sp;
		auto& core = GetCoreComponent();
		const auto& library = Description::Library::Get();
		const auto& spec = library.ProgramSpecs[core.SpecificationID];

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << GetUUID();
		emitter << YAML::Key << "Source Filepath" << YAML::Value << GetSourceFilepath()->Filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str();
		emitter << YAML::Key << "Usage" << YAML::Value << core.Usage.ToConstCharPtr();
		emitter << YAML::Key << "Specification" << YAML::Value << spec.UUID;
		emitter << YAML::Key << "Width" << YAML::Value << core.Width;
		emitter << YAML::Key << "Height" << YAML::Value << core.Height;
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
			!node["Specification"].IsDefined() ||
			!node["Width"].IsDefined() ||
			!node["Height"].IsDefined() ||
			!node["Source Filepath"].IsDefined())
		{
			FE_LOG_CORE_ERROR("Ill defined Texture in serialized node");
			return false;
		}

		auto& core = GetCoreComponent();
		
		core.Usage.FromString(node["Usage"].as<std::string>());

		auto& lib = Description::Library::Get();
		auto spec_uuid = node["Specification"].as<UUID>();
		core.SpecificationID = lib.CreateOrGetDescriptorWithUUID<Description::ShaderInterface::Specification>(spec_uuid);

		core.Width = node["Width"].as<uint32_t>();
		core.Height = node["Height"].as<uint32_t>();

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
			!node["Specification"].IsDefined() ||
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


		core.Usage.FromString(node["Usage"].as<std::string>());

		auto& lib = Description::Library::Get();
		auto spec_uuid = node["Specification"].as<UUID>();
		core.SpecificationID = lib.CreateOrGetDescriptorWithUUID<Description::ShaderInterface::Specification>(spec_uuid);
		
		core.Width = node["Width"].as<uint32_t>();
		core.Height = node["Height"].as<uint32_t>();

		std::filesystem::path source_path = parentpath;
		source_path /= node["Source Filepath"].as<std::string>();
		AssetManager::SetSourcePath(asset_id, source_path);

		return asset_id;
	}
}