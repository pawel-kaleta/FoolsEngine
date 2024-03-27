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
		static const std::unordered_map<std::string, Ref<Material>>& GetAll() { return s_ActiveInstance->IGetAll(); }
	private:
		friend class Renderer;

		static MaterialLibrary* s_ActiveInstance;
		std::unordered_map<std::string, Ref<Material>> m_Materials;
		
		void IAdd(const Ref<Material> material);

		bool IExist(const std::string& name) const;
		Ref<Material> IGet(const std::string& name);
		const std::unordered_map<std::string, Ref<Material>>& IGetAll() { return m_Materials; }
	};

	class MaterialInstanceLibrary
	{
	public:
		static void Add(Ref<MaterialInstance> instance) { s_ActiveInstance->IAdd(instance); }

		static bool Exist(const std::string& name) { return s_ActiveInstance->IExist(name); }
		static Ref<MaterialInstance> Get(const std::string& name) { return s_ActiveInstance->IGet(name); }

		static void SetActiveInstance(MaterialInstanceLibrary* materialInstanceLib) { s_ActiveInstance = materialInstanceLib; }
		static const std::unordered_map<std::string, Ref<MaterialInstance>>& GetAll() { return s_ActiveInstance->IGetAll(); }

		static void Rename(Ref<MaterialInstance> instance, const std::string& newName) { s_ActiveInstance->IRename(instance, newName); }
	private:
		friend class Renderer;

		static MaterialInstanceLibrary* s_ActiveInstance;
		std::unordered_map<std::string, Ref<MaterialInstance>> m_Instances;

		void IAdd(const Ref<MaterialInstance> instance);

		bool IExist(const std::string& name) const;
		Ref<MaterialInstance> IGet(const std::string& name);
		void IRename(Ref<MaterialInstance> instance, const std::string& newName);
		const std::unordered_map<std::string, Ref<MaterialInstance>>& IGetAll() { return m_Instances; }
	};
}