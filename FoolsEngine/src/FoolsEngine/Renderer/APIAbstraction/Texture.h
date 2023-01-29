#pragma once

#include "FoolsEngine\Renderer\GDIType.h"

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

		virtual const std::string& GetName() const = 0;
	};

	class TextureLibrary
	{
	public:
		inline static void Add(Ref<Texture> texture) { s_ActiveInstance->IAdd(texture); }

		inline static bool Exist(const std::string& name) { return s_ActiveInstance->IExist(name); }
		inline static Ref<Texture> Get(const std::string& name) { return s_ActiveInstance->IGet(name); }

		inline static void SetActiveInstance(TextureLibrary* textureLib) { s_ActiveInstance = textureLib; }

		void IAdd(const Ref<Texture> texture);

		bool IExist(const std::string& name) const;
		Ref<Texture> IGet(const std::string& name);

	private:
		static TextureLibrary* s_ActiveInstance;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;
	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		virtual TextureType GetType() const override { return TextureType::Texture2D; }

		static Scope<Texture> Create(const std::string& filePath, GDIType GDI);
		static Scope<Texture> Create(const std::string& filePath);
	};
}