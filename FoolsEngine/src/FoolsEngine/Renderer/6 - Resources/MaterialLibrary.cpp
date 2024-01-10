#include "FE_pch.h"

#include "MaterialLibrary.h"

namespace fe
{
	MaterialLibrary* MaterialLibrary::s_ActiveInstance = nullptr;

	void MaterialLibrary::IAdd(const Ref<Material> material)
	{
		const std::string& name = material->GetName();
		FE_CORE_ASSERT(!IExist(name), "Material already in library!");
		m_Materials[name] = material;
	}

	bool MaterialLibrary::IExist(const std::string& name) const
	{
		return m_Materials.find(name) != m_Materials.end();;
	}

	Ref<Material> MaterialLibrary::IGet(const std::string& name)
	{
		FE_CORE_ASSERT(IExist(name), "Texture not found!");
		return m_Materials[name];
	}
}