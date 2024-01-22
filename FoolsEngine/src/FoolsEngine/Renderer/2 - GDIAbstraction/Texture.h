#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\1 - Primitives\TextureData.h"

namespace fe
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual TextureData::Format GetFormat() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual TextureData::Type GetType() const = 0;
		virtual uint32_t GetID() const = 0;

		virtual const std::string& GetName() const = 0;
		virtual const std::string& GetFilePath() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		virtual TextureData::Type GetType() const override { return TextureData::Type::Texture2D; }

		static Scope<Texture> Create(const std::string& name, uint32_t width, uint32_t hight);
		static Scope<Texture> Create(const std::string& name, uint32_t width, uint32_t hight, GDIType GDI);
		static Scope<Texture> Create(const std::string& filePath, GDIType GDI);
		static Scope<Texture> Create(const std::string& filePath);
	};
}