#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"

#include "FoolsEngine\Assets\AssetManager.h"

namespace fe
{
	struct ACSourceCode final : public AssetComponent
	{
		std::string VertexSource;
		std::string FragmentSource;
		std::string ShaderSource;
	};

	class Shader : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static AssetType GetTypeStatic() { return AssetType::ShaderAsset; }

		const ACSourceCode* GetSourceCode() const { return GetIfExist<ACSourceCode>(); }
		      ACSourceCode* GetSourceCode()       { return GetIfExist<ACSourceCode>(); }
		ACSourceCode& GetOrEmplaceSourceCode() { return GetOrEmplace<ACSourceCode>(); }

		template <typename tnGDIShader, typename... Args>
		tnGDIShader& CreateGDIShader(Args&&... args) { return Emplace<tnGDIShader>(std::forward<Args>(args)...); }

		void Bind(GDIType GDI);
		void Unbind(GDIType GDI);

		virtual void UnloadFromGPU() override final;
		virtual void UnloadFromCPU() override final;

		uint32_t GetRendererID(GDIType GDI) const;

		void UploadUniform(GDIType GDI, const Uniform& uniform, void* dataPointer, uint32_t count = 1, bool transpose = false);
		void BindTextureSlot(GDIType GDI, const ShaderTextureSlot& textureSlot, RenderTextureSlotID* rendererTextureSlot, uint32_t count);
		void BindTextureSlot(GDIType GDI, const ShaderTextureSlot& textureSlot, RenderTextureSlotID rendererTextureSlot);

	protected:
		Shader(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) {}
	};
}
