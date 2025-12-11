#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\2 - Resource\Shader.h"
#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"


namespace YAML { class Emitter; }

namespace fe
{
	struct ACShaderCore final : public AssetComponent
	{
		uint32_t SpecificationID;

		void Init();
	};

	struct ACShaderResource_OpenGL final : public AssetComponent
	{
		Resource::Shader_OpenGL Shader;
	};

	class ShaderObserver : public AssetInterface
	{
	public:
		const ACShaderCore& GetCoreComponent() const { return Get<ACShaderCore>(); }
		
		template <GAPIType::ValueType GAPI>
		const auto& GetResourceComponent()
		{
			if constexpr (GAPI == GAPIType::OpenGL) return Get<ACShaderResource_OpenGL>();
		}

		void SaveMetadata(YAML::Emitter& emitter);

	protected:
		ShaderObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {};
	};

	class ShaderUser : public ShaderObserver
	{
	public:
		ACShaderCore& GetCoreComponent() const { return Get<ACShaderCore>(); }

		template <typename tnGAPIShader, typename... Args>
		tnGAPIShader& CreateResourceComponent(Args&&... args) const { return Emplace<tnGAPIShader>(std::forward<Args>(args)...); }

		bool LoadMetadata();

		void Release() const;

		void SendDataToGPU(GAPIType GAPI, void* data);
		void UnloadFromCPU() const;
	protected:
		ShaderUser(ECS_AssetHandle ECS_handle) : ShaderObserver(ECS_handle) {}
	};

	class Shader : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Shader; }
		static constexpr const char* GetMetaFileExtension() { return ""; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACShaderCore>(assetID).Init(); }

		using Observer = ShaderObserver;
		using User = ShaderUser;
		using Core = ACShaderCore;
	};
}
