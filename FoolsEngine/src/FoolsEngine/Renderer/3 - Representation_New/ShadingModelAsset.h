#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\1 - Description\Buffer.h"
#include "FoolsEngine\Renderer\1 - Description\ShaderInterface.h"
#include "FoolsEngine\Renderer\2 - Resource\Program.h"

#include "FoolsEngine\Assets\AssetInterface.h"
#include "FoolsEngine\Assets\AssetHandle.h"

namespace YAML { class Emitter; }

namespace fe
{
	struct ACShadingModelAssetCore final : public AssetComponent
	{
		AssetID VertexShaderID;
		AssetID FragmentShaderID;
		uint32_t ProgramSpecificationID;

		void* DefaultUniformsData;
		size_t UniformsDataSize;

		void Init();

		~ACShadingModelAssetCore() { if (DefaultUniformsData) operator delete(DefaultUniformsData); }
	};

	struct ACShaderModelResource_OpenGL final : public AssetComponent
	{
		Resource::Program_OpenGL Program;
	};

	class ShadingModelAssetObserver : public AssetInterface
	{
	public:
		const ACShadingModelAssetCore& GetCoreComponent() const { return Get<ACShadingModelAssetCore>(); }

		template <GAPIType::ValueType GAPI>
		const auto& GetResourceComponent()
		{
			if constexpr (GAPI == GAPIType::OpenGL) return Get<ACShaderModelResource_OpenGL>();
		}

		const void* GetUniformDefaultValuePtr(const ACShadingModelAssetCore& dataComponent, const Description::Buffer::Element& targetUniform) const { return GetUniformDefaultValuePtr_Internal(dataComponent, targetUniform); };
		const void* GetUniformDefaultValuePtr(const ACShadingModelAssetCore& dataComponent, const std::string& name) const { return GetUniformDefaultValuePtr_Internal(dataComponent, name); };

		void SaveMetadata(YAML::Emitter& emitter);
		bool LoadMetadata();

	protected:
		ShadingModelAssetObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}

		void* GetUniformDefaultValuePtr_Internal(const ACShadingModelAssetCore& dataComponent, const Description::Buffer::Element& targetUniform) const;
		void* GetUniformDefaultValuePtr_Internal(const ACShadingModelAssetCore& dataComponent, const std::string& name) const;
	};

	class ShadingModelAssetUser : public ShadingModelAssetObserver
	{
	public:
		ACShadingModelAssetCore& GetCoreComponent() const { return Get<ACShadingModelAssetCore>(); }

		void* GetUniformDefaultValuePtr(const ACShadingModelAssetCore& dataComponent, const Description::Buffer::Element& targetUniform) const { return GetUniformDefaultValuePtr_Internal(dataComponent, targetUniform); };
		void* GetUniformDefaultValuePtr(const ACShadingModelAssetCore& dataComponent, const std::string& name) const { return GetUniformDefaultValuePtr_Internal(dataComponent, name); };

		void SetUniformDefaultValue(const ACShadingModelAssetCore& dataComponent, const Description::Buffer::Element& uniform, void* dataPointer) const;
		void SetUniformDefaultValue(const ACShadingModelAssetCore& dataComponent, const std::string& name, void* dataPointer) const;

	protected:
		ShadingModelAssetUser(ECS_AssetHandle ECS_handle) : ShadingModelAssetObserver(ECS_handle) {}
	};

	class ShadingModelAsset : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ShadingModel; }
		static constexpr const char* GetMetaFileExtension() { return ".fesm"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACShadingModelAssetCore>(assetID).Init(); }

		using User = ShadingModelAssetUser;
		using Observer = ShadingModelAssetObserver;
		using Core = ACShadingModelAssetCore;

	private:
		friend class Renderer;

		static bool DeserializeFromFile(AssetID assetID, const std::filesystem::path& filepath);
	};
}