#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\IndexBuffer.h"

#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\Transform.h"

// fix this, higher layer should not be accesed
#include "FoolsEngine\Resources\TextureLibrary.h"
#include "FoolsEngine\Resources\ShaderLibrary.h"
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
		Mesh(
			UUID uuid,
			const std::filesystem::path& filepath,
			const std::string& name,
			uint32_t assimpMaterialIndex,
			uint32_t assimpMeshIndex,
			Ref<MaterialInstance> materialInstance)
			:
			m_UUID(uuid),
			m_Filepath(filepath),
			m_Name(name),
			m_AssimpMaterialIndex(assimpMaterialIndex),
			m_AssimpMeshIndex(assimpMeshIndex),
			m_MaterialInstance(materialInstance)
		{ }

		const std::filesystem::path& GetFilepath()    const { return m_Filepath; }
		const std::string&           GetName()        const { return m_Name; }
		      UUID                   GetUUID()        const { return m_UUID; }
			  size_t                 GetVertexCount() const;

		enum DataLocation
		{
			None = 0,
			CPU,
			GPU,
			CPU_GPU
		};

		// swaps vectors' data with internal vectors
		Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, Ref<MaterialInstance> materialInstance);

		void UploadBuffersToGPU();
		void FreeBuffersFromCPU();
		void FreeBuffersFromGPU();

		void Draw(Ref<MaterialInstance> materialInstance);
		void Draw() { Draw(m_MaterialInstance); }
	private:
		friend class ModelImporter;

		std::string m_Name;
		std::filesystem::path m_Filepath;
		UUID m_UUID;

		std::vector<Vertex>   m_Vertices;
		std::vector<uint32_t> m_Indices;
		uint32_t m_AssimpMaterialIndex = -1;
		uint32_t m_AssimpMeshIndex = -1;
		
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer>  m_IndexBuffer;
		DataLocation m_DataLocation = DataLocation::None;

		Ref<MaterialInstance> m_MaterialInstance;
	};

	class MeshBuilder
	{
	public:
		MeshBuilder& SetName(const std::string& name) { m_Name = name; return *this; }
		MeshBuilder& SetFilepath(const std::filesystem::path& filepath) { m_Filepath = filepath; return *this; }
		MeshBuilder& SetUUID(UUID uuid) { m_UUID = uuid; return *this; }
		MeshBuilder& SetAssimpMaterialIndex(uint32_t assimpMaterialIndex) { m_AssimpMaterialIndex = assimpMaterialIndex; return *this; }
		MeshBuilder& SetAssimpMeshIndex(uint32_t assimpMeshIndex) { m_AssimpMeshIndex = assimpMeshIndex; return *this; }
		MeshBuilder& SetMaterialInstance(Ref<MaterialInstance> materialInstance) { m_MaterialInstance = materialInstance; }

		Ref<Mesh> Build()
		{
			return Ref<Mesh>(new Mesh(
				m_UUID,
				m_Filepath,
				m_Name,
				m_AssimpMaterialIndex,
				m_AssimpMeshIndex,
				m_MaterialInstance
			));
		}
	private:
		std::string m_Name;
		std::filesystem::path m_Filepath;
		UUID m_UUID;

		uint32_t m_AssimpMaterialIndex = -1;
		uint32_t m_AssimpMeshIndex = -1;

		Ref<MaterialInstance> m_MaterialInstance;
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