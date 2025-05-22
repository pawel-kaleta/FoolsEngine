#pragma once

#include "FoolsEngine\Assets\Asset.h"

namespace fe
{
	enum class GDIType;
	class Uniform;
	class ShaderTextureSlot;
	using RenderTextureSlotID = uint32_t;

	struct ACSourceCode final : public AssetComponent
	{
		std::string VertexSource;
		std::string FragmentSource;
		std::string ShaderSource;

		void Init();
	};

	class ShaderObserver : public AssetInterface
	{
	public:
		virtual AssetType GetType() const override final { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::ShaderAsset; }

		const ACSourceCode* GetSourceCode() const { return GetIfExist<ACSourceCode>(); }

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
		ACSourceCode* GetSourceCode() const { return GetIfExist<ACSourceCode>(); }
		ACSourceCode& GetOrEmplaceSourceCode() const { return GetOrEmplace<ACSourceCode>(); }

		template <typename tnGDIShader, typename... Args>
		tnGDIShader& CreateGDIShader(Args&&... args) const { return Emplace<tnGDIShader>(std::forward<Args>(args)...); }

		void PlaceCoreComponent() const { };
		void Release()  const;

		void SendDataToGPU(GDIType GDI, void* data) { FE_CORE_ASSERT(false, "Shader loading not implemented yet"); };
		void UnloadFromCPU() const;
	protected:
		ShaderUser(ECS_AssetHandle ECS_handle) : ShaderObserver(ECS_handle) {}
	};

	class Shader : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ShaderAsset; }

		using Observer = ShaderObserver;
		using User = ShaderUser;
	};
}
