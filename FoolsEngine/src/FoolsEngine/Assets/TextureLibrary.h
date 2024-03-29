#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"

namespace fe
{
	class TextureLibrary
	{
	public:
		static void Add(Ref<Texture> texture) { s_ActiveInstance->IAdd(texture); }

		static bool Exist(const std::string& name) { return s_ActiveInstance->IExist(name); }
		static Ref<Texture> Get(const std::string& name) { return s_ActiveInstance->IGet(name); }

		static void SetActiveInstance(TextureLibrary* textureLib) { s_ActiveInstance = textureLib; }

		static const std::unordered_map<std::string, Ref<Texture>>& GetAll() { return s_ActiveInstance->IGetAll(); }
	private:
		static TextureLibrary* s_ActiveInstance;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;

		friend class Renderer;
		bool IExist(const std::string& name) const;
		void IAdd(const Ref<Texture> texture);
		Ref<Texture> IGet(const std::string& name);
		const std::unordered_map<std::string, Ref<Texture>>& IGetAll() { return m_Textures; }
	};
}

