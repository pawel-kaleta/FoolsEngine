#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\IndexBuffer.h"

#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\Transform.h"

// TO DO: fix this, higher layer should not be accesed
#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"

#include "FoolsEngine\Scene\ECS.h"

#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetHandle.h"

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

	class Mesh : public Asset
	{
	public:
		Mesh() = default;
		Mesh(
			AssetSignature* assetSignature,
			const std::string& name,
			uint32_t assimpMaterialIndex,
			uint32_t assimpMeshIndex,
			AssetHandle<MaterialInstance> materialInstance)
			:
			Asset(assetSignature),
			m_Name(name),
			m_AssimpMaterialIndex(assimpMaterialIndex),
			m_AssimpMeshIndex(assimpMeshIndex),
			m_MaterialInstance(materialInstance)
		{ }

		static AssetType GetAssetType() { return AssetType::MeshAsset; }

		const std::string& GetName() const { return m_Name; }
		size_t GetVertexCount() const;

		enum DataLocation
		{
			None = 0,
			CPU,
			GPU,
			CPU_GPU
		};

		// swaps vectors' data with internal vectors
		Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, AssetHandle<MaterialInstance> materialInstance);

		void UploadBuffersToGPU();
		void FreeBuffersFromCPU();
		void FreeBuffersFromGPU();

		void Draw(AssetHandle<MaterialInstance> materialInstance);
		void Draw() { Draw(m_MaterialInstance); }
	private:
		friend class ModelImporter;

		std::string m_Name;

		std::vector<Vertex>   m_Vertices;
		std::vector<uint32_t> m_Indices;
		uint32_t m_AssimpMaterialIndex = -1;
		uint32_t m_AssimpMeshIndex = -1;
		
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer>  m_IndexBuffer;
		DataLocation m_DataLocation = DataLocation::None;

		AssetHandle<MaterialInstance> m_MaterialInstance;
	};

	class MeshBuilder
	{
	public:
		MeshBuilder& SetName(const std::string& name) { m_Name = name; return *this; }
		MeshBuilder& SetSignature(AssetSignature* signature) { m_AssetSignature = signature; }
		MeshBuilder& SetAssimpMaterialIndex(uint32_t assimpMaterialIndex) { m_AssimpMaterialIndex = assimpMaterialIndex; return *this; }
		MeshBuilder& SetAssimpMeshIndex(uint32_t assimpMeshIndex) { m_AssimpMeshIndex = assimpMeshIndex; return *this; }
		MeshBuilder& SetMaterialInstance(AssetHandle<MaterialInstance> materialInstance) { m_MaterialInstance = materialInstance; }

		Ref<Mesh> Build()
		{
			return Ref<Mesh>(new Mesh(
				m_AssetSignature,
				m_Name,
				m_AssimpMaterialIndex,
				m_AssimpMeshIndex,
				m_MaterialInstance
			));
		}
	private:
		std::string m_Name;
		AssetSignature* m_AssetSignature;

		uint32_t m_AssimpMaterialIndex = -1;
		uint32_t m_AssimpMeshIndex = -1;

		AssetHandle<MaterialInstance> m_MaterialInstance;
	};

	class Model
	{
	public:


	//private:
		std::string m_Name;
		std::filesystem::path m_Filepath;
		UUID m_UUID;

		std::vector<Ref<Mesh>> m_Meshes;
		std::vector<Transform> m_Transforms;
	};
}