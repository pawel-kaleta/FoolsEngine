#include "MeshImport.h"

#include <FoolsEngine.h>

#include <filesystem>

namespace fe
{
	std::filesystem::path MeshImport::s_Filepath;
	AssetHandle<Mesh>* s_Handle;
	static char str0[128];

	void MeshImport::RenderWindow()
	{
		
	}

	void MeshImport::InitImport(const std::filesystem::path& filepath, AssetHandleBase* optionalBaseHandle)
	{
		s_Handle = (AssetHandle<Mesh>*)optionalBaseHandle;
		s_Filepath = filepath;

		memset(str0, 0, sizeof(str0));
		strncpy_s(str0, "Texture name", sizeof(str0));
	}

	void MeshImport::ImportAs(const std::filesystem::path& filepath)
	{

	}
}