#pragma once

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"

namespace YAML { class Emitter; }

namespace fe::Render::Representation
{
	struct ACShader_Core final : public AssetComponent
	{
		GAPI::Descriptors::ShaderType mType;

		void Init()
		{
			mType = GAPI::Descriptors::ShaderType::None;
		}
	};

	template <GAPI::Platform::ValueType tPlatform>
	struct ACShader_GID final : public AssetComponent
	{
		GAPI::GID mShaderGID;
	};

	class ShaderObserver : public AssetInterface
	{
	public:
		const ACShader_Core& GetCore() const { return Get<ACShader_Core>(); }

	protected:
		ShaderObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {};
	};

	class ShaderUser : public ShaderObserver
	{
	public:
		ACShader_Core& GetCore() const { return Get<ACShader_Core>(); }

	protected:
		ShaderUser(ECS_AssetHandle ECS_handle) : ShaderObserver(ECS_handle) {}
	};

	class Shader : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Shader; }
		static constexpr const char* GetMetaFileExtension() { return ""; }

		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACShader_Core>(assetID).Init(); }

		using Observer = ShaderObserver;
		using User = ShaderUser;
		using Core = ACShader_Core;
	};
}
