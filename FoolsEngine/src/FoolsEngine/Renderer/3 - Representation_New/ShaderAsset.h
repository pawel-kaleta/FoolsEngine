#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\2 - Resource\Shader.h"
#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"


namespace YAML { class Emitter; }

namespace fe
{
	struct ACShaderAssetCore final : public AssetComponent
	{
		//std::string ShaderSource;
		uint32_t SpecificationID;

		void Init();
	};

	struct ACShaderResource_OpenGL final : public AssetComponent
	{
		Resource::Shader_OpenGL Shader;
	};

	class ShaderAssetObserver : public AssetInterface
	{
	public:
		const ACShaderAssetCore& GetCoreComponent() const { return Get<ACShaderAssetCore>(); }
		
		template <GAPIType::ValueType GAPI>
		const auto& GetResourceComponent()
		{
			if constexpr (GAPI == GAPIType::OpenGL) return Get<ACShaderResource_OpenGL>();
		}

		void SaveMetadata(YAML::Emitter& emitter);
		bool LoadMetadata();

	protected:
		ShaderAssetObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {};
	};

	class ShaderAssetUser : public ShaderAssetObserver
	{
	public:
		ACShaderAssetCore& GetCoreComponent() const { return Get<ACShaderAssetCore>(); }

		template <typename tnGAPIShader, typename... Args>
		tnGAPIShader& CreateResourceComponent(Args&&... args) const { return Emplace<tnGAPIShader>(std::forward<Args>(args)...); }

		void Release() const;

		void SendDataToGPU(GAPIType GAPI, void* data);
		void UnloadFromCPU() const;
	protected:
		ShaderAssetUser(ECS_AssetHandle ECS_handle) : ShaderAssetObserver(ECS_handle) {}
	};

	class ShaderAsset : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ShaderAsset; }
		static constexpr const char* GetMetaFileExtension() { return ""; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACShaderAssetCore>(assetID).Init(); }

		using Observer = ShaderAssetObserver;
		using User = ShaderAssetUser;
		using Core = ACShaderAssetCore;
	};
}
