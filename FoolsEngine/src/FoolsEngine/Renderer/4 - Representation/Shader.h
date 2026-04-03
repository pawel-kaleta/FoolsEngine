#pragma once

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/2 - Resource/Shader.h"

namespace YAML { class Emitter; }

namespace fe
{
	struct ACShaderCore final : public AssetComponent
	{
		uint32_t SpecificationID;
		std::string ShaderSource;

		void Init();
	};

	struct ACShaderResource_OpenGL final : public AssetComponent
	{
		Resource::Shader_OpenGL Shader;
	};

	class ShaderObserver : public AssetInterface
	{
	public:
		const ACShaderCore& GetCore() const { return Get<ACShaderCore>(); }
		const ACShaderResource_OpenGL* GetResourceOpenGL() const { return GetIfExist<ACShaderResource_OpenGL>(); }

		void SaveMetadata(YAML::Emitter& emitter);

	protected:
		ShaderObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {};
	};

	class ShaderUser : public ShaderObserver
	{
	public:
		ACShaderCore& GetCore() const { return Get<ACShaderCore>(); }
		ACShaderResource_OpenGL* GetResourceOpenGL() const { return GetIfExist<ACShaderResource_OpenGL>(); }

		bool LoadMetadata();

		void UnloadFromCPU() const;
		void Release() const;

	protected:
		ShaderUser(ECS_AssetHandle ECS_handle) : ShaderObserver(ECS_handle) {}
	};

	class Shader : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Shader; }
		static constexpr const char* GetMetaFileExtension() { return ""; }
		static void SaveMetadata(YAML::Emitter& emitter, AssetID assetID) {}
		static bool LoadMetadata(AssetID assetID) { return false; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACShaderCore>(assetID).Init(); }

		using Observer = ShaderObserver;
		using User = ShaderUser;
		using Core = ACShaderCore;
	};
}
