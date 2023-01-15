#pragma once

namespace fe
{
	enum class TextureType
	{
		None = 0,
		Texture2D
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		//virtual TextureFormat GetFormat() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual TextureType GetType() const = 0;

		static Scope<Texture> s_DefaultTexture;
	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		virtual TextureType GetType() const override { return TextureType::Texture2D; }

		static Scope<Texture> Create(const std::string& filePath);
	};
}