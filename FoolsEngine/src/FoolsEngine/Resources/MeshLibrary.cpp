#include "FE_pch.h"
#include "MeshLibrary.h"

namespace fe
{
	MeshLibrary* MeshLibrary::s_ActiveInstance = nullptr;

	void MeshLibrary::IAdd(const Ref<Mesh> mesh)
	{
		const std::string& name = mesh->GetName();
		FE_CORE_ASSERT(!IExist(name), "Material already in library!");
		m_Meshes[name] = mesh;
	}

	bool MeshLibrary::IExist(const std::string& name) const
	{
		return m_Meshes.find(name) != m_Meshes.end();;
	}

	Ref<Mesh> MeshLibrary::IGet(const std::string& name)
	{
		FE_CORE_ASSERT(IExist(name), "Texture not found!");
		return m_Meshes[name];
	}
}