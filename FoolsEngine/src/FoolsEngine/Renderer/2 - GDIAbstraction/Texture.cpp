#include "FE_pch.h"
#include "Texture.h"

#include "OpenGL\OpenGLTexture.h"

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Assets\AssetAccessors.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Core\Project.h"

#include "FoolsEngine\Memory\Scratchpad.h"

namespace fe
{
	bool Texture2DUser::SendDataToGPU(GDIType GDI) const
	{
		FE_PROFILER_FUNC();

		auto& core = GetCoreComponent();

		if (!core.Data)
			return false;

		switch (GDI.Value)
		{
		case GDIType::None:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			break;

		case GDIType::OpenGL:
			auto& spec = Get<ACTexture2DCore>();
			Get<OpenGLTexture2D>().SendDataToGPU(core.Data, spec.Specification);
			break;
		}

		return true;
	}

	void Texture2DUser::Bind(GDIType GDI, RenderTextureSlotID slotID) const
	{
		switch (GDI.Value)
		{
		case GDIType::None:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			break;

		case GDIType::OpenGL:
			auto ptr = GetIfExist<OpenGLTexture2D>();
			FE_CORE_ASSERT(ptr, "Trying to bind texture not loaded to GPU");
			if (ptr)
				ptr->Bind(slotID);
			break;
		}
	}

	void Texture2DUser::UnloadFromCPU() const
	{
		FE_PROFILER_FUNC();

		auto& dataPtr = Get<ACTexture2DCore>().Data;
		if (dataPtr)
		{
			TextureLoader::UnloadTexture(dataPtr);
			dataPtr = nullptr;
		}
	}

	uint32_t Texture2DObserver::GetRendererID(GDIType GDI) const
	{
		switch (GDI.Value)
		{
		case GDIType::None:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			return 0;

		case GDIType::OpenGL:
			auto ptr = GetIfExist<OpenGLTexture2D>();
			if (ptr)
				return ptr->GetOpenGLID();
			return 0;
		}
		return 0;
	}

	void Texture2DUser::CreateGDITexture2D(GDIType gdi) const
	{
		FE_PROFILER_FUNC();
		auto& data = Get<ACTexture2DCore>();
		CreateGDITexture2D(gdi, data.Specification, data.Data);
	}

	void Texture2DUser::CreateGDITexture2D(GDIType gdi, const TextureData::Specification& spec, const void* data) const
	{
		FE_PROFILER_FUNC();

		switch (gdi.Value)
		{
		case GDIType::None:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			break;

		case GDIType::OpenGL:
			Emplace<OpenGLTexture2D>(spec, data);
			break;
		}
	}

	void Texture2DUser::Release() const
	{
		FE_PROFILER_FUNC();

		auto gdi = Renderer::GetActiveGDItype();
		switch (gdi.Value)
		{
		case GDIType::None:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			break;

		case GDIType::OpenGL:
			if (AllOf<OpenGLTexture2D>())
			{
				FE_LOG_CORE_DEBUG("Unloading Texture from GPU, AssetID: {0}, RendererID: {1}", GetID(), GetRendererID(gdi));
				Erase<OpenGLTexture2D>();
			}
			break;
		}
	}

	void Texture2D::SaveMetadata(YAML::Emitter& emitter, AssetID assetID)
	{
		FE_PROFILER_FUNC();

		auto assetObserver = AssetObserver<Texture2D>(assetID);
		
		Scratchpad sp;

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << assetObserver.GetUUID();
		emitter << YAML::Key << "Source Filepath" << YAML::Value << assetObserver.GetSourceFilepath()->Filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str();
		auto& spec = assetObserver.GetCoreComponent().Specification;
		emitter << YAML::Key << "Usage" << YAML::Value << spec.Usage.ToConstCharPtr();
		emitter << YAML::Key << "Components" << YAML::Value << spec.Components.ToConstCharPtr();
		emitter << YAML::Key << "Format" << YAML::Value << spec.Format.ToConstCharPtr();
		emitter << YAML::Key << "Width" << YAML::Value << spec.Width;
		emitter << YAML::Key << "Height" << YAML::Value << spec.Height;
		emitter << YAML::EndMap;
	}

	bool Texture2D::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();

		auto filepath = Project::Get()->m_AssetsPath;
		const auto& relative_path = reg.get<ACFilepath>(assetID).Filepath;
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
			!node["Components"].IsDefined() ||
			!node["Format"].IsDefined() ||
			!node["Width"].IsDefined() ||
			!node["Height"].IsDefined() ||
			!node["Source Filepath"].IsDefined())
		{
			FE_LOG_CORE_ERROR("Ill defined Texture in serialized node");
			return false;
		}

		auto& spec = reg.get<Texture2D::Core>(assetID).Specification;
		
		spec.Usage.FromString(node["Usage"].as<std::string>());
		spec.Components.FromString(node["Components"].as<std::string>());
		spec.Format.FromString(node["Format"].as<std::string>());
		spec.Width = node["Width"].as<uint32_t>();
		spec.Height = node["Height"].as<uint32_t>();

		std::filesystem::path source_path = node["Source Filepath"].as<std::string>();
		AssetManager::SetSourcePath(assetID, source_path);

		return true;
	}

	AssetID Texture2D::LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentpath)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();

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
			!node["Components"].IsDefined() ||
			!node["Format"].IsDefined() ||
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

		auto& spec = reg.get<ACTexture2DCore>(asset_id).Specification;
		spec.Usage.FromString(node["Usage"].as<std::string>());
		spec.Components.FromString(node["Components"].as<std::string>());
		spec.Format.FromString(node["Format"].as<std::string>());
		spec.Width = node["Width"].as<uint32_t>();
		spec.Height = node["Height"].as<uint32_t>();

		std::filesystem::path source_path = parentpath;
		source_path /= node["Source Filepath"].as<std::string>();
		AssetManager::SetSourcePath(asset_id, source_path);

		return asset_id;
	}
}