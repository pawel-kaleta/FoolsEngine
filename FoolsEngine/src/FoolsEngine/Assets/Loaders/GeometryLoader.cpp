#include "FE_pch.h"
#include "GeometryLoader.h"

#include "FoolsEngine\Renderer\1 - Primitives\VertexData.h"
#include "FoolsEngine\Core\Project.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace fe
{
	//void GeometryLoader::UnloadModel(void* data)
	//{
	//	FE_CORE_ASSERT(false, "Not implemented");
	//}

	bool GeometryLoader::IsKnownExtension(const std::pmr::string& extension)
	{
		constexpr static const char * knownExtensions[] = {
			".obj",
			".fbx",
			".glb",
			".gltf"
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

	bool GeometryLoader::IsKnownAssetType(AssetType assetType)
	{
		constexpr static const AssetType knownTypes[] = {
			AssetType::Mesh,
			AssetType::RenderMesh,
			AssetType::Model
		};

		for (const auto& knownType : knownTypes)
		{
			if (knownType == assetType)
			{
				return true;
			}
		}

		return false;
	}

	constexpr static uint32_t s_assimp_load_flags =
		aiPostProcessSteps::aiProcess_Triangulate |
		aiPostProcessSteps::aiProcess_RemoveRedundantMaterials |
		aiPostProcessSteps::aiProcess_JoinIdenticalVertices |
		aiPostProcessSteps::aiProcess_PreTransformVertices |
		aiPostProcessSteps::aiProcess_FindInstances |
		aiPostProcessSteps::aiProcess_ImproveCacheLocality |
		aiPostProcessSteps::aiProcess_OptimizeMeshes |
		aiPostProcessSteps::aiProcess_GenSmoothNormals |
		aiPostProcessSteps::aiProcess_CalcTangentSpace |
		aiPostProcessSteps::aiProcess_GenUVCoords |
		aiPostProcessSteps::aiProcess_TransformUVCoords;

	const aiScene* GeometryLoader::InspectSourceFile(const std::filesystem::path& filePath)
	{
		static Assimp::Importer s_Inspector;
		
		const aiScene* scene = s_Inspector.ReadFile(filePath.string().c_str(), s_assimp_load_flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			FE_LOG_CORE_ERROR("ERROR::ASSIMP::{0}", s_Inspector.GetErrorString());
			return nullptr;
		}

		return scene;
	}
	
	void GeometryLoader::LoadMesh(const std::filesystem::path& sourceFilePath, AssetUser<Mesh>& meshUser)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile((Project::GetInstance()->AssetsPath / sourceFilePath).string().c_str(), s_assimp_load_flags);

		auto& mesh_core_component = meshUser.GetCoreComponent();

		auto& dataLocation = mesh_core_component.Data;
		if (dataLocation)
		{
			FE_LOG_CORE_WARN("Reloading mesh");
			delete dataLocation;
		}

		auto& spec = mesh_core_component.Specification;
		
		dataLocation = (void*) new float[mesh_core_component.DataSize() / sizeof(float)];
		auto last = (float*)dataLocation + (mesh_core_component.DataSize() / sizeof(float));
		uint32_t* first_index = (uint32_t*)dataLocation;
		uint32_t* index_ptr = (uint32_t*)dataLocation;
		VertexData::Vertex* first_vertex_ptr = (VertexData::Vertex*)(index_ptr + spec.IndexCount);
		VertexData::Vertex* vertex_ptr = first_vertex_ptr;

		uint32_t index_offset = 0;

		for (size_t j = 0; j < scene->mNumMeshes; j++)
		{
			auto& assimpMesh = scene->mMeshes[j];
			bool sndUV = assimpMesh->GetNumUVChannels() > 1;

			for (size_t i = 0; i < assimpMesh->mNumVertices; i++)
			{
				auto& vertex = *vertex_ptr;

				vertex.Position.x = assimpMesh->mVertices[i].x;
				vertex.Position.y = assimpMesh->mVertices[i].y;
				vertex.Position.z = assimpMesh->mVertices[i].z;

				vertex.Normal.x = assimpMesh->mNormals[i].x;
				vertex.Normal.y = assimpMesh->mNormals[i].y;
				vertex.Normal.z = assimpMesh->mNormals[i].z;

				vertex.Tangent.x = assimpMesh->mTangents[i].x;
				vertex.Tangent.y = assimpMesh->mTangents[i].y;
				vertex.Tangent.z = assimpMesh->mTangents[i].z;

				vertex.UV0.x = assimpMesh->mTextureCoords[0][i].x;
				vertex.UV0.y = assimpMesh->mTextureCoords[0][i].y;

				if (sndUV)
				{
					vertex.UV1.x = assimpMesh->mTextureCoords[1][i].x;
					vertex.UV1.y = assimpMesh->mTextureCoords[1][i].y;
				}
				else
				{
					vertex.UV1.x = assimpMesh->mTextureCoords[0][i].x;
					vertex.UV1.y = assimpMesh->mTextureCoords[0][i].y;
				}

				vertex_ptr++;
			}

			for (size_t i = 0; i < assimpMesh->mNumFaces; i++)
			{
				aiFace& face = assimpMesh->mFaces[i];
				
				*(index_ptr + 0) = (uint32_t)(face.mIndices[0] + index_offset);
				*(index_ptr + 1) = (uint32_t)(face.mIndices[1] + index_offset);
				*(index_ptr + 2) = (uint32_t)(face.mIndices[2] + index_offset);

				index_ptr += 3;
			}
			index_offset = (uint32_t)(vertex_ptr - first_vertex_ptr);
		}

		return;
	}

	//void GeometryLoader::LoadModel(const std::filesystem::path& sourceFilePath, AssetUser<Model>& modelUser)
	//{
	//	FE_LOG_CORE_WARN("Not implemented yet - geometry loader model loading");
	//}
}