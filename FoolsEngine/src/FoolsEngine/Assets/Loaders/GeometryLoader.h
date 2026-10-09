#pragma once

#include "FoolsEngine/Assets/Loaders/LoaderType.h"

#include "FoolsEngine/Renderer/2 - Representation/Mesh.h"
#include "FoolsEngine/Renderer/2 - Representation/Model.h"

struct aiScene;

namespace fe
{
	class GeometryLoader
	{
	public:
		static constexpr LoaderType GetTypeStatic() { return LoaderType::Geometry; }
		static constexpr AssetType GetAssetTypeStatic() { return AssetType::None; }
		static void LoadMesh(const std::filesystem::path& sourceFilePath, AssetUser& meshUser);
		static void LoadMesh(AssetUser& meshUser)
		{
			auto path = meshUser.GetSourceFilepath()->Filepath;
			LoadMesh(path, meshUser);
		}
		static void UnloadMesh(void* data) { delete[] (float*)data; }

		static void LoadModel(const std::filesystem::path& sourceFilePath, AssetUser& modelUser);
		static void LoadModel(AssetUser& modelUser)
		{
			auto path = modelUser.GetSourceFilepath()->Filepath;
			LoadModel(path, modelUser);
		}
		//static void UnloadModel(void* data);

		static bool IsKnownExtension(const std::pmr::string& extension);
		static bool IsKnownAssetType(AssetType assetType);
		static const char* GetExtensionAlias() { return "Geometry Source"; }
		static const aiScene* InspectSourceFile(const std::filesystem::path& filePath);
	};
}