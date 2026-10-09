#pragma once

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"

namespace YAML { class Emitter; }

namespace fe
{
	struct ACShader_Core final : public AssetComponent
	{
		Render::GAPI::Descriptors::ShaderType mType;

		void Init()
		{
			mType = Render::GAPI::Descriptors::ShaderType::None;
		}
	};

	template <Render::GAPI::Platform::ValueType tPlatform>
	struct ACShader_GID final : public AssetComponent
	{
		Render::GAPI::GID mShaderGID;
	};

	class Shader : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Shader; }
		static constexpr const char* GetMetaFileExtension() { return ""; }

		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACShader_Core>(assetID).Init(); }

		using Core = ACShader_Core;
	};
}
