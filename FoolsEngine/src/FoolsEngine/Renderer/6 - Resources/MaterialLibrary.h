#pragma once

#include "FoolsEngine\Renderer\3 - Representation\Material.h"

namespace fe
{
	class MaterialLibrary
	{
	public:
		static void Add(Ref<Material> material) { s_ActiveInstance->IAdd(material); }

		static bool Exist(const std::string& name) { return s_ActiveInstance->IExist(name); }
		static Ref<Material> Get(const std::string& name) { return s_ActiveInstance->IGet(name); }

		static void SetActiveInstance(MaterialLibrary* materialLib) { s_ActiveInstance = materialLib; }

		void IAdd(const Ref<Material> material);

		bool IExist(const std::string& name) const;
		Ref<Material> IGet(const std::string& name);

	private:
		static MaterialLibrary* s_ActiveInstance;
		std::unordered_map<std::string, Ref<Material>> m_Materials;
	};
}