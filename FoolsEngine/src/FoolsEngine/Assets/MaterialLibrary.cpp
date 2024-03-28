#include "FE_pch.h"

#include "MaterialLibrary.h"

namespace fe
{
	MaterialLibrary* MaterialLibrary::s_ActiveInstance = nullptr;
	MaterialInstanceLibrary* MaterialInstanceLibrary::s_ActiveInstance = nullptr;

	void MaterialLibrary::IAdd(const Ref<Material> material)
	{
		const std::string& name = material->GetName();
		FE_CORE_ASSERT(!IExist(name), "Material already in library!");
		m_Materials[name] = material;
	}

	bool MaterialLibrary::IExist(const std::string& name) const
	{
		return m_Materials.find(name) != m_Materials.end();
	}

	Ref<Material> MaterialLibrary::IGet(const std::string& name)
	{
		FE_CORE_ASSERT(IExist(name), "Texture not found!");
		return m_Materials[name];
	}

	void MaterialInstanceLibrary::IAdd(const Ref<MaterialInstance> instance)
	{
		const std::string& name = instance->GetName();
		FE_CORE_ASSERT(!IExist(name), "Material already in library!");
		m_Instances[name] = instance;
	}

	bool MaterialInstanceLibrary::IExist(const std::string& name) const
	{
		return m_Instances.find(name) != m_Instances.end();;
	}

	Ref<MaterialInstance> MaterialInstanceLibrary::IGet(const std::string& name)
	{
		FE_CORE_ASSERT(IExist(name), "Texture not found!");
		return m_Instances[name];
	}

	void MaterialInstanceLibrary::IRename(Ref<MaterialInstance> instance, const std::string& newName)
	{
		m_Instances.erase(instance->GetName());
		instance->SetName(newName);
		IAdd(instance);
	}
}