#pragma once

#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"

namespace fe
{
	class MeshLoader
	{
	public:
		static void LoadMesh(const std::filesystem::path& sourceFilePath, AssetUser<Mesh>& meshUser);
		static void LoadMesh(AssetUser<Mesh>& meshUser)
		{
			LoadMesh(meshUser.GetSourceFilepath(), meshUser);
		}
		static bool IsKnownExtension(const std::filesystem::path& extension);

	private:
		struct ImportMesh
		{
			std::vector<Vertex>   Vertices;
			std::vector<uint32_t> Indices;
			uint32_t AssimpMaterialIndex = -1;
			uint32_t AssimpMeshIndex = -1;

		};

		struct ImportData
		{
			const aiScene* Scene = nullptr;
			std::filesystem::path Directory;
			std::vector<Ref<ImportMesh>> Meshes;
		};

		static void ProcessNode(aiNode* node, ImportData& importData);

		static void ProcessMesh(aiMesh* newMesh, ImportMesh* meshBatch);
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