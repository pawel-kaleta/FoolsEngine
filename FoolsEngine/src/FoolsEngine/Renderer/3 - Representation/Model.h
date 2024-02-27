#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\IndexBuffer.h"

#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\Transform.h"

// fix this, higher layer should not be accesed
#include "FoolsEngine\Renderer\6 - Resources\TextureLibrary.h"
#include "FoolsEngine\Renderer\6 - Resources\ShaderLibrary.h"
#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"

#include "FoolsEngine\Scene\ECS.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace fe
{
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	class Mesh
	{
	public:
		Mesh() = default;

		enum DataLocation
		{
			None = 0,
			CPU,
			GPU,
			CPU_GPU
		};

		// swaps vectors' data with internal vectors
		Mesh(
			Ref<MaterialInstance> materialInstance,
			std::vector<Vertex>& vertices,
			std::vector<uint32_t>& indices
		);

		void UploadBuffersToGPU();

		void FreeBuffersFromCPU();

		void FreeBuffersFromGPU();

		void Draw(Transform& transform, glm::mat4 VP, EntityID entityID);

		static Ref<Material> Default3DMaterial;
		Ref<MaterialInstance> m_MaterialInstance;
	private:
		// mesh data
		std::vector<Vertex>   m_Vertices;
		std::vector<uint32_t> m_Indices;

		std::array<Ref<Texture>, 4>  m_Textures;
		std::array<ShaderTextureSlot, 4> m_TextureSlots = {
			ShaderTextureSlot("u_Albedo"   , TextureData::Type::Texture2D),
			ShaderTextureSlot("u_Roughness", TextureData::Type::Texture2D),
			ShaderTextureSlot("u_Metalness", TextureData::Type::Texture2D),
			ShaderTextureSlot("u_AO"       , TextureData::Type::Texture2D)
		};
		
		// render data
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer>  m_IndexBuffer;
		DataLocation m_DataLocation = DataLocation::None;
	};

	class Model
	{
	public:
		Model() = default;
		Model(std::filesystem::path path) { LoadModel(path); }

		void Draw(Transform& transform, glm::mat4 VP, EntityID entityID)
		{
			for (unsigned int i = 0; i < m_Meshes.size(); i++)
			{
				m_Meshes[i].Draw(transform, VP, entityID);
			}
		}

		void LoadModel(std::filesystem::path path)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				FE_LOG_CORE_ERROR("ERROR::ASSIMP::{0}", importer.GetErrorString());
				return;
			}
			m_Directory = path.remove_filename();

			ProcessNode(scene->mRootNode, scene);

			for (auto& mesh : m_Meshes)
			{
				mesh.UploadBuffersToGPU();
				mesh.FreeBuffersFromCPU();
			}
		}
	private:
		std::vector<Mesh> m_Meshes;
		std::filesystem::path m_Directory;

		void ProcessNode(aiNode* node, const aiScene* scene)
		{
			// process all the node's meshes (if any)
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				m_Meshes.push_back(ProcessMesh(mesh, scene));
			}
			// then do the same for each of its children
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				ProcessNode(node->mChildren[i], scene);
			}
		}

		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
			Ref<MaterialInstance> materialInstance;
			//std::array<Ref<Texture>, 4> textures;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				// process vertex positions, normals and texture coordinates

				vertex.Position.x = mesh->mVertices[i].x;
				vertex.Position.y = mesh->mVertices[i].y;
				vertex.Position.z = mesh->mVertices[i].z;

				vertex.Normal.x = mesh->mNormals[i].x;
				vertex.Normal.y = mesh->mNormals[i].y;
				vertex.Normal.z = mesh->mNormals[i].z;

				if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
				{
					vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
					vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
				}
				else
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);

				vertices.push_back(vertex);
			}

			// process indices
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			
			// process material
			if (mesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				auto name = material->GetName();

				for (auto& mesh : m_Meshes)
				{
					if (mesh.m_MaterialInstance->GetName() == name.C_Str())
					{
						materialInstance = mesh.m_MaterialInstance;
					}
				}

				if (!materialInstance)
				{
					if (m_Meshes.size())
					{
						FE_LOG_CORE_ERROR("Multiple materials in a model not supported yet");
						return Mesh();
					}
					materialInstance.reset(new MaterialInstance(Mesh::Default3DMaterial, name.C_Str()));
				}
			}

			return Mesh(materialInstance, vertices, indices);
		}

		Ref<Texture> LoadMaterialTexture(aiMaterial* mat, aiTextureType type, TextureData::Usage usage)
		{
			Ref<Texture> texture;

			if (mat->GetTextureCount(type) == 0)
				return texture;

			if (mat->GetTextureCount(type) > 1)
				FE_LOG_CORE_ERROR("Multiple textures of the same usage for mesh");

			aiString str;
			mat->GetTexture(type, 0, &str);
			auto textureName = FileNameFromFilepath(str.C_Str());

			if (TextureLibrary::Exist(textureName))
				return TextureLibrary::Get(textureName);

			std::string textureFilePath = m_Directory.string() + str.C_Str();
			texture = Texture2D::Create(textureFilePath, usage);
			TextureLibrary::Add(texture);
			return texture;
		}
	};
}