#include "FE_pch.h"
#include "MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace fe
{
	
	//const aiScene* s_InspectedScene = nullptr;

	bool MeshLoader::IsKnownExtension(const std::filesystem::path& extension)
	{
		static std::filesystem::path knownExtensions[] = {
			".obj",
			".fbx"
		};

		for (auto& knownExtension : knownExtensions)
		{
			if (extension == knownExtension)
			{
				return true;
			}
		}

		return false;
	}

	const aiScene* MeshLoader::InspectSourceFile(const std::filesystem::path& filePath)
	{
		static Assimp::Importer s_Inspector;

		const aiScene* scene = s_Inspector.ReadFile(filePath.string().c_str(),
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenNormals |
			aiProcess_ValidateDataStructure);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			FE_LOG_CORE_ERROR("ERROR::ASSIMP::{0}", s_Inspector.GetErrorString());
			return nullptr;
		}

		return scene;
	}
	
	void MeshLoader::LoadMesh(const std::filesystem::path& sourceFilePath, AssetUser<Mesh>& meshUser)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(sourceFilePath.string().c_str(),
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenNormals |
			aiProcess_ValidateDataStructure);

		ImportData importData;
		importData.Scene = scene;
		importData.Directory = sourceFilePath;
		importData.Directory.remove_filename();

		auto& dataLocation = meshUser.GetDataLocation();
		if (dataLocation.Data)
		{
			FE_LOG_CORE_WARN("Reloading mesh");
			delete dataLocation.Data;
		}

		dataLocation.Data = new MeshData();

		ProcessNode(scene->mRootNode, importData, meshUser.GetSpecification(), (MeshData*)dataLocation.Data);

		return;
	}

	void MeshLoader::ProcessNode(aiNode* node, ImportData& importData, const ACMeshSpecification* meshSpec, MeshData* meshData)
	{
		// process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* newMesh = importData.Scene->mMeshes[node->mMeshes[i]];
			//ImportMesh* meshBatch = importData.Meshes[newMesh->mMaterialIndex].get();

			if (meshSpec->AssimpMaterialIndex != newMesh->mMaterialIndex)
				continue;

			ProcessMesh(newMesh, meshData);
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], importData, meshSpec, meshData);
		}
	}

	void MeshLoader::ProcessMesh(aiMesh* newMesh, MeshData* meshData)
	{
		uint32_t newMeshIndicesOffset = (uint32_t)meshData->Vertices.size();

		for (unsigned int i = 0; i < newMesh->mNumVertices; i++)
		{
			Vertex vertex;

			vertex.Position.x = newMesh->mVertices[i].x;
			vertex.Position.y = newMesh->mVertices[i].y;
			vertex.Position.z = newMesh->mVertices[i].z;

			vertex.Normal.x = newMesh->mNormals[i].x;
			vertex.Normal.y = newMesh->mNormals[i].y;
			vertex.Normal.z = newMesh->mNormals[i].z;

			if (newMesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				vertex.TexCoords.x = newMesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = newMesh->mTextureCoords[0][i].y;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			meshData->Vertices.push_back(vertex);
		}

		// process indices
		for (unsigned int i = 0; i < newMesh->mNumFaces; i++)
		{
			aiFace face = newMesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				meshData->Indices.push_back(face.mIndices[j] + newMeshIndicesOffset);
		}
	}

	/*
	
	void ModelImporter::Import(Ref<Mesh> mesh)
	{
		Assimp::Importer importer;
		//aiProcessPreset_TargetRealtime_Fast
		const aiScene* scene = importer.ReadFile(path.string().c_str(),
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenNormals |
			aiProcess_ValidateDataStructure);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			FE_LOG_CORE_ERROR("ERROR::ASSIMP::{0}", importer.GetErrorString());
			return Ref<Mesh>();
		}

		ImportData importData;
		importData.Scene = scene;
		importData.Directory = path;
		importData.Directory.remove_filename();

		Mesh* mesh = new Mesh();
		mesh->m_Filepath = path;
		//mesh->
		mesh->m_Name = FileNameFromFilepath(path.string()) + "__" + scene->mMeshes[assimpMeshIndex]->mName.C_Str();
		mesh->m_AssimpMaterialIndex = scene->mMeshes[assimpMeshIndex]->mMaterialIndex;
		mesh->m_AssimpMeshIndex = assimpMeshIndex;



		ProcessNode(scene->mRootNode, importData);

		for (auto& mesh : importData.MeshBatches)
		{
			mesh->m_DataLocation = Mesh::DataLocation::CPU;
			mesh->UploadBuffersToGPU();
			mesh->FreeBuffersFromCPU();
		}

		return importData.MeshBatches;
		
	}


	Ref<Mesh> ModelImporter::Import(const std::filesystem::path& path, uint32_t assimpMeshIndex)
	{
		Assimp::Importer importer;
		//aiProcessPreset_TargetRealtime_Fast
		const aiScene* scene = importer.ReadFile(path.string().c_str(),
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenNormals |
			aiProcess_ValidateDataStructure);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			FE_LOG_CORE_ERROR("ERROR::ASSIMP::{0}", importer.GetErrorString());
			return Ref<Mesh>();
		}

		ImportData importData;
		importData.Scene = scene;
		importData.Directory = path;
		importData.Directory.remove_filename();

		importData.Meshes.reserve(scene->mNumMaterials);
		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			Mesh* meshBatch = new Mesh();
			meshBatch->m_Filepath = path;
			meshBatch->m_Name = FileNameFromFilepath(path.string()) + "__" + scene->mMaterials[i]->GetName().C_Str();
			meshBatch->m_AssimpMaterialIndex = i;

			importData.MeshBatches.push_back(Ref<Mesh>(meshBatch));
		}

		ProcessNode(scene->mRootNode, importData);

		for (auto& mesh : importData.MeshBatches)
		{
			mesh->m_DataLocation = Mesh::DataLocation::CPU;
			mesh->UploadBuffersToGPU();
			mesh->FreeBuffersFromCPU();
		}

		return importData.MeshBatches;
	}
	
	void ModelImporter::ProcessNode(aiNode* node, ImportData& importData)
	{
		// process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* newMesh = importData.Scene->mMeshes[node->mMeshes[i]];
			Mesh* meshBatch = importData.MeshBatches[newMesh->mMaterialIndex].get();
			ProcessMesh(newMesh, meshBatch);
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], importData);
		}
	}

	void ModelImporter::ProcessMesh(aiMesh* newMesh, Mesh* meshBatch)
	{
		uint32_t newMeshIndicesOffset = meshBatch->m_Vertices.size();

		for (unsigned int i = 0; i < newMesh->mNumVertices; i++)
		{
			Vertex vertex;

			vertex.Position.x = newMesh->mVertices[i].x;
			vertex.Position.y = newMesh->mVertices[i].y;
			vertex.Position.z = newMesh->mVertices[i].z;

			vertex.Normal.x = newMesh->mNormals[i].x;
			vertex.Normal.y = newMesh->mNormals[i].y;
			vertex.Normal.z = newMesh->mNormals[i].z;

			if (newMesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				vertex.TexCoords.x = newMesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = newMesh->mTextureCoords[0][i].y;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			meshBatch->m_Vertices.push_back(vertex);
		}

		// process indices
		for (unsigned int i = 0; i < newMesh->mNumFaces; i++)
		{
			aiFace face = newMesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				meshBatch->m_Indices.push_back(face.mIndices[j] + newMeshIndicesOffset);
		}
	}

	*/
}