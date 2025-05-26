#include "FE_pch.h"
#include "GeometryLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace fe
{
	void GeometryLoader::UnloadModel(void* data)
	{
		FE_CORE_ASSERT(false, "Not implemented");

	}

	bool GeometryLoader::IsKnownExtension(const std::pmr::string& extension)
		{
			static std::pmr::string knownExtensions[] = {
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

	const aiScene* GeometryLoader::InspectSourceFile(const std::filesystem::path& filePath, uint32_t loadFlags)
		{
			static Assimp::Importer s_Inspector;
			loadFlags |= 
				aiProcess_Triangulate |
				aiProcess_ValidateDataStructure |
				aiProcess_RemoveRedundantMaterials |
				aiProcess_JoinIdenticalVertices |
				aiProcess_ImproveCacheLocality |
				aiProcess_GenSmoothNormals |
				aiProcess_CalcTangentSpace |
				aiProcess_GenUVCoords |
				aiProcess_TransformUVCoords |
				aiProcess_FlipUVs;

			const aiScene* scene = s_Inspector.ReadFile(filePath.string().c_str(), loadFlags);

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
		const aiScene* scene = importer.ReadFile(sourceFilePath.string().c_str(),
			aiProcess_Triangulate |
			aiProcess_ValidateDataStructure |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_JoinIdenticalVertices |
			aiProcess_PreTransformVertices |
			aiProcess_ImproveCacheLocality |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_GenUVCoords |
			aiProcess_TransformUVCoords |
			aiProcess_FlipUVs 
		);

		auto& mesh_core_component = meshUser.GetCoreComponent();

		auto& dataLocation = mesh_core_component.Data;
		if (dataLocation)
		{
			FE_LOG_CORE_WARN("Reloading mesh");
			delete dataLocation;
		}

		auto& spec = mesh_core_component.Specification;
		
		dataLocation = (void*) new float[mesh_core_component.DataSize() / sizeof(float)];
		float* index_ptr = (float*)dataLocation;
		Vertex* first_vertex_ptr = (Vertex*)(index_ptr + spec.IndexCount);
		Vertex* vertex_ptr = first_vertex_ptr;

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
				
				index_ptr[0] = face.mIndices[0] + index_offset;
				index_ptr[1] = face.mIndices[1] + index_offset;
				index_ptr[2] = face.mIndices[2] + index_offset;

				index_ptr += 3;
			}
			index_offset = vertex_ptr - first_vertex_ptr;
		}

		return;
	}

	void GeometryLoader::LoadModel(const std::filesystem::path& sourceFilePath, AssetUser<Model>& modelUser)
	{
		FE_LOG_CORE_WARN("Not implemented yet - geometry loader model loading");
	}
}