#include "FE_pch.h"
#include "Texture.h"

#include "OpenGL\OpenGLTexture.h"

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Assets\AssetAccessors.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

namespace fe
{
	void Texture2DUser::SendDataToGPU(GDIType GDI, void* data) const
	{
		switch (GDI.Value)
		{
		case GDIType::None:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			break;

		case GDIType::OpenGL:
			auto& spec = Get<ACTexture2DCore>();
			Get<OpenGLTexture2D>().SendDataToGPU(data, spec.Specification);
			break;
		}
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
		auto& data = Get<ACTexture2DCore>();
		CreateGDITexture2D(gdi, data.Specification, data.Data);
	}

	void Texture2DUser::CreateGDITexture2D(GDIType gdi, const TextureData::Specification& spec, const void* data) const
	{
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

	void Texture2D::Serialize(const AssetObserver<Texture2D>& assetObserver)
	{
		YAML::Emitter emitter;

		emitter << YAML::Key << "UUID" << YAML::Value << assetObserver.GetUUID();
		emitter << YAML::Key << "Source Filepath" << YAML::Value << assetObserver.GetSourceFilepath()->Filepath.string();
		emitter << YAML::Key << "Specification" << YAML::Value << YAML::BeginMap;
		auto& spec = assetObserver.GetCoreComponent().Specification;
			emitter << YAML::Key << "Usage" << YAML::Value << spec.Usage.ToString();
			emitter << YAML::Key << "Components" << YAML::Value << spec.Components.ToString();
			emitter << YAML::Key << "Format" << YAML::Value << spec.Format.ToString();
			emitter << YAML::Key << "Width" << YAML::Value << spec.Width;
			emitter << YAML::Key << "Height" << YAML::Value << spec.Height;
			emitter << YAML::EndMap;

		std::ofstream fout(assetObserver.GetFilepath());
		fout << emitter.c_str();
	}

	bool Texture2D::Deserialize(AssetID assetID)
	{
		auto& reg = AssetManager::GetRegistry();
		ECS_AssetHandle ECS_handle(reg, assetID);

		auto& filepath = ECS_handle.get<ACFilepath>().Filepath;

		YAML::Node node = YAML::LoadFile(filepath.string());

		auto uuid_node = node["UUID"];
		if (!uuid_node) return false;
		if (ECS_handle.get<ACUUID>().UUID != node["UUID"].as<UUID>())
		{
			FE_CORE_ASSERT(false, "Not machting UUID in asset and its metafile!");
			return false;
		}

		auto source_node = node["Source Filepath"];
		if (!source_node) return false;
		AssetManager::SetSourcePath(assetID, source_node.as<std::string>());

		auto spec_node = node["Specification"];
		if (!spec_node) return false;
		if (!spec_node["Usage"]) return false;
		if (!spec_node["Components"]) return false;
		if (!spec_node["Format"]) return false;
		if (!spec_node["Width"]) return false;
		if (!spec_node["Height"]) return false;

		auto& spec = ECS_handle.get<ACTexture2DCore>().Specification;
		spec.Usage.FromString(spec_node["Usage"].as<std::string>());
		spec.Components.FromString(spec_node["Components"].as<std::string>());
		spec.Format.FromString(spec_node["Format"].as<std::string>());
		spec.Width = spec_node["Width"].as<uint32_t>();
		spec.Height = spec_node["Height"].as<uint32_t>();

		return true;
	}
}