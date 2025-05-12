#pragma once

#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"
#include "FoolsEngine\Renderer\3 - Representation\Model.h"

struct aiScene;

namespace fe
{
	class GeometryLoader
	{
	public:
		static void LoadMesh(const std::filesystem::path& sourceFilePath, AssetUser<Mesh>& meshUser);
		static void LoadMesh(AssetUser<Mesh>& meshUser)
		{
			auto path = meshUser.GetFilepath();
			LoadMesh(path.Filepath, meshUser);
		}
		static void UnloadMesh(void* data) { delete[] (uint32_t*)data; }

		static void LoadModel(const std::filesystem::path& sourceFilePath, AssetUser<Model>& modelUser);
		static void LoadModel(AssetUser<Model>& modelUser)
		{
			auto path = modelUser.GetFilepath();
			LoadModel(path.Filepath, modelUser);
		}
		static void UnloadModel(void* data);

		static bool IsKnownExtension(const std::pmr::string& extension);
		static const char* GetExtensionAlias() { return "Geometry Source"; }
		static const aiScene* InspectSourceFile(const std::filesystem::path& filePath, uint32_t loadFlags = 0);
	};
}