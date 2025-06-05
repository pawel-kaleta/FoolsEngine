#pragma once

#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"

namespace fe
{
	struct GDIType;
	class Uniform;
	class ShaderTextureSlot;
	using RenderTextureSlotID = uint32_t;

	struct ACShaderCore final : public AssetComponent
	{
		std::string VertexSource;
		std::string FragmentSource;
		std::string ShaderSource;

		void Init();
	};

	class ShaderObserver : public AssetInterface
	{
	public:
		const ACShaderCore& GetCoreComponent() const { return Get<ACShaderCore>(); }

		uint32_t GetRendererID(GDIType GDI) const;

		void Bind(GDIType GDI) const;
		void Unbind(GDIType GDI) const;

		void UploadUniform(GDIType GDI, const Uniform& uniform, const void* dataPointer, uint32_t count = 1, bool transpose = false) const;
		void BindTextureSlot(GDIType GDI, const ShaderTextureSlot& textureSlot, RenderTextureSlotID* rendererTextureSlot, uint32_t count) const;
		void BindTextureSlot(GDIType GDI, const ShaderTextureSlot& textureSlot, RenderTextureSlotID rendererTextureSlot) const;
	protected:
		ShaderObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class ShaderUser : public ShaderObserver
	{
	public:
		ACShaderCore& GetCoreComponent() const { return Get<ACShaderCore>(); }

		template <typename tnGDIShader, typename... Args>
		tnGDIShader& CreateGDIShader(Args&&... args) const { return Emplace<tnGDIShader>(std::forward<Args>(args)...); }

		void Release() const;

		void SendDataToGPU(GDIType GDI, void* data);;
		void UnloadFromCPU() const;
	protected:
		ShaderUser(ECS_AssetHandle ECS_handle) : ShaderObserver(ECS_handle) {}
	};

	template<typename>
	class AssetObserver;

	class Shader : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ShaderAsset; }
		static void EmplaceCore(AssetID assetID) { AssetManager::GetRegistry().emplace<ACShaderCore>(assetID).Init(); }
		static void Serialize(const AssetObserver<Shader>& assetObserver) { };
		static bool Deserialize(AssetID assetID) { return true; };

		using Observer = ShaderObserver;
		using User = ShaderUser;
		using Core = ACShaderCore;
	};
}
