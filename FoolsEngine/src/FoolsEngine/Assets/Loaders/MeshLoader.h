#pragma once

#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"

namespace fe
{
	class MeshLoader
	{
	public:
		static void LoadTexture(const std::filesystem::path& sourceFilePath, AssetUser<Mesh>& textureUser);
		static void LoadTexture(AssetUser<Mesh>& textureUser)
		{
			LoadTexture(textureUser.GetSourceFilepath(), textureUser);
		}
		static bool IsKnownExtension(const std::filesystem::path& extension);
	};

	/*

	class ModelImporter
	{
	public:
		static void Import(Ref<Mesh> mesh);

	private:
		ModelImporter() = default;

		struct ImportData
		{
			const aiScene* Scene = nullptr;
			std::filesystem::path Directory;
			std::vector<Ref<Mesh>> Meshes;
		};

		static void ProcessNode(aiNode* node, ImportData& importData);

		static void ProcessMesh(aiMesh* newMesh, Mesh* meshBatch);
	};

	class MeshLoader
	{
	public:
		static void Load(Ref<Mesh> mesh);

	private:
		MeshLoader() = default;

		struct LoadData
		{
			const aiScene* Scene = nullptr;
			std::filesystem::path Directory;
		};


	};


	*/
}