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
		constexpr static const char * s_known_extensions[] = {
			".obj",
			".fbx",
			".glb",
			".gltf"
		};

		for (auto& known_extension : s_known_extensions)
		{
			if (extension == known_extension)
			{
				return true;
			}
		}

		return false;
	}

	bool GeometryLoader::IsKnownAssetType(AssetType assetType)
	{
		constexpr static const AssetType s_known_types[] = {
			AssetType::Mesh,
			AssetType::RenderMesh,
			AssetType::Model
		};

		for (const auto& known_type : s_known_types)
		{
			if (known_type == assetType)
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
		FE_PROFILER_FUNC();

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile((Project::Get()->m_AssetsPath / sourceFilePath).string().c_str(), s_assimp_load_flags);

		auto& mesh_core_component = meshUser.GetCoreComponent();

		auto& data_location = mesh_core_component.Data;
		if (data_location)
		{
			FE_LOG_CORE_WARN("Reloading mesh");
			delete data_location;
		}

		auto& spec = mesh_core_component.Specification;
		
		data_location = (void*) new float[mesh_core_component.DataSize() / sizeof(float)];
		auto last = (float*)data_location + (mesh_core_component.DataSize() / sizeof(float));
		uint32_t* first_index = (uint32_t*)data_location;
		uint32_t* index_ptr = (uint32_t*)data_location;
		VertexData::Vertex* first_vertex_ptr = (VertexData::Vertex*)(index_ptr + spec.IndexCount);
		VertexData::Vertex* vertex_ptr = first_vertex_ptr;

		uint32_t index_offset = 0;

		for (size_t j = 0; j < scene->mNumMeshes; j++)
		{
			auto& assimp_mesh = scene->mMeshes[j];
			bool sndUV = assimp_mesh->GetNumUVChannels() > 1;

			for (size_t i = 0; i < assimp_mesh->mNumVertices; i++)
			{
				auto& vertex = *vertex_ptr;

				vertex.Position.x = assimp_mesh->mVertices[i].x;
				vertex.Position.y = assimp_mesh->mVertices[i].y;
				vertex.Position.z = assimp_mesh->mVertices[i].z;

				vertex.Normal.x = assimp_mesh->mNormals[i].x;
				vertex.Normal.y = assimp_mesh->mNormals[i].y;
				vertex.Normal.z = assimp_mesh->mNormals[i].z;

				vertex.Tangent.x = assimp_mesh->mTangents[i].x;
				vertex.Tangent.y = assimp_mesh->mTangents[i].y;
				vertex.Tangent.z = assimp_mesh->mTangents[i].z;

				vertex.UV0.x = assimp_mesh->mTextureCoords[0][i].x;
				vertex.UV0.y = assimp_mesh->mTextureCoords[0][i].y;

				if (sndUV)
				{
					vertex.UV1.x = assimp_mesh->mTextureCoords[1][i].x;
					vertex.UV1.y = assimp_mesh->mTextureCoords[1][i].y;
				}
				else
				{
					vertex.UV1.x = assimp_mesh->mTextureCoords[0][i].x;
					vertex.UV1.y = assimp_mesh->mTextureCoords[0][i].y;
				}

				vertex_ptr++;
			}

			for (size_t i = 0; i < assimp_mesh->mNumFaces; i++)
			{
				aiFace& face = assimp_mesh->mFaces[i];
				
				*(index_ptr + 0) = (uint32_t)(face.mIndices[0] + index_offset);
				*(index_ptr + 1) = (uint32_t)(face.mIndices[1] + index_offset);
				*(index_ptr + 2) = (uint32_t)(face.mIndices[2] + index_offset);

				index_ptr += 3;
			}
			index_offset = (uint32_t)(vertex_ptr - first_vertex_ptr);
		}

		return;
	}

	void GeometryLoader::LoadModel(const std::filesystem::path& sourceFilePath, AssetUser<Model>& modelUser)
	{
		FE_PROFILER_FUNC();

		auto& base_path = Project::Get()->m_AssetsPath;
		auto direct_path = base_path / sourceFilePath;

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(direct_path.string().c_str(), s_assimp_load_flags);

		const auto& render_meshes = modelUser.GetCoreComponent().RenderMeshIDs;

		for (size_t i = 0; i < render_meshes.size(); i++)
		{
			auto mesh_id = AssetObserver<RenderMesh>(render_meshes[i]).GetCoreComponent().MeshID;
			AssetUser<Mesh> mesh_user(mesh_id);

			auto& core = mesh_user.GetCoreComponent();

			auto& data_location = core.Data;
			if (data_location)
			{
				FE_LOG_CORE_WARN("Reloading mesh");
				delete[] data_location;
			}

			auto& spec = core.Specification;

			data_location = (void*) new float[core.DataSize() / sizeof(float)];

			uint32_t* index_ptr = (uint32_t*)data_location;
			VertexData::Vertex* vertex_ptr = (VertexData::Vertex*)(index_ptr + spec.IndexCount);

			auto& assimp_mesh = scene->mMeshes[i];
			bool sndUV = assimp_mesh->GetNumUVChannels() > 1;

			for (size_t j = 0; j < assimp_mesh->mNumVertices; j++)
			{
				auto& vertex = *vertex_ptr;

				vertex.Position.x = assimp_mesh->mVertices[j].x;
				vertex.Position.y = assimp_mesh->mVertices[j].y;
				vertex.Position.z = assimp_mesh->mVertices[j].z;

				vertex.Normal.x = assimp_mesh->mNormals[j].x;
				vertex.Normal.y = assimp_mesh->mNormals[j].y;
				vertex.Normal.z = assimp_mesh->mNormals[j].z;

				vertex.Tangent.x = assimp_mesh->mTangents[j].x;
				vertex.Tangent.y = assimp_mesh->mTangents[j].y;
				vertex.Tangent.z = assimp_mesh->mTangents[j].z;

				vertex.UV0.x = assimp_mesh->mTextureCoords[0][j].x;
				vertex.UV0.y = assimp_mesh->mTextureCoords[0][j].y;

				if (sndUV)
				{
					vertex.UV1.x = assimp_mesh->mTextureCoords[1][j].x;
					vertex.UV1.y = assimp_mesh->mTextureCoords[1][j].y;
				}
				else
				{
					vertex.UV1.x = assimp_mesh->mTextureCoords[0][j].x;
					vertex.UV1.y = assimp_mesh->mTextureCoords[0][j].y;
				}

				vertex_ptr++;
			}

			for (size_t j = 0; j < assimp_mesh->mNumFaces; j++)
			{
				aiFace& face = assimp_mesh->mFaces[j];

				*(index_ptr + 0) = (uint32_t)(face.mIndices[0]);
				*(index_ptr + 1) = (uint32_t)(face.mIndices[1]);
				*(index_ptr + 2) = (uint32_t)(face.mIndices[2]);

				index_ptr += 3;
			}
		}

		return;
	}
}