#pragma once

#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"

struct aiScene;

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
		static const aiScene* InspectSourceFile(const std::filesystem::path& filePath);

	private:
		struct ImportData
		{
			const aiScene* Scene = nullptr;
			std::filesystem::path Directory;
			//std::vector<MeshData> Meshes;
		};

		static void ProcessNode(aiNode* node, ImportData& importData, const ACMeshSpecification* meshSpec, MeshData* meshData);

		static void ProcessMesh(aiMesh* newMesh, MeshData* meshData);
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