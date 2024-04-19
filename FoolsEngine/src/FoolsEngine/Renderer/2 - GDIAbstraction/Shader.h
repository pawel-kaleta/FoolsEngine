#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"

#include "FoolsEngine\Assets\Asset.h"

namespace fe
{
	class Shader : public Asset
	{
	public:
		virtual ~Shader() = default;

		static AssetType GetAssetType() { return AssetType::ShaderAsset; }

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void UploadUniform(const Uniform& uniform, void* dataPointer, uint32_t count = 1, bool transpose = false) = 0;
		virtual void BindTextureSlot(const ShaderTextureSlot& textureSlot, uint32_t* rendererTextureSlot, uint32_t count) = 0;
		virtual void BindTextureSlot(const ShaderTextureSlot& textureSlot, uint32_t rendererTextureSlot) = 0;

		virtual const std::string& GetName() const = 0;
		virtual const uint32_t& GetProgramID() const = 0;

		static Shader* Create(AssetSignature* assetSignature, const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, GDIType GDI);
		static Shader* Create(AssetSignature* assetSignature, const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		static Shader* Create(AssetSignature* assetSignature, const std::string& name, const std::string& shaderSource, GDIType GDI);
		static Shader* Create(AssetSignature* assetSignature, const std::string& name, const std::string& shaderSource);
		static Shader* Create(AssetSignature* assetSignature, const std::filesystem::path& filePath, GDIType GDI);
		static Shader* Create(AssetSignature* assetSignature, const std::filesystem::path& filePath);
	private:

	};

	
}
