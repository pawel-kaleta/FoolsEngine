#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\IndexBuffer.h"

#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\MaterialInstance.h"

// TO DO: fix this, higher layer should not be accesed
#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"

#include "FoolsEngine\Scene\ECS.h"

#include "FoolsEngine\Math\Transform.h"
#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetHandle.h"

#include "FoolsEngine/Core/Core.h"

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

	struct ACMaterialInstance final : public AssetComponent
	{
		AssetHandle<MaterialInstance> MaterialInstance;
	};

	struct MeshData //ACDataLocation
	{
		std::vector<Vertex>   Vertices;
		std::vector<uint32_t> Indices;
	};

	struct ACMeshSpecification final : public AssetComponent
	{
		uint32_t AssimpMaterialIndex = -1;
		std::vector<uint32_t> AssimpMeshesIndexes;
		uint32_t VertexCount = 0;
		uint32_t IndicesCount = 0;
	};

	struct ACGPUBuffers final : public AssetComponent
	{
		Ref<VertexBuffer> VertexBuffer;
		Ref<IndexBuffer>  IndexBuffer;
	};

	class Mesh : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		constexpr static AssetType GetTypeStatic() { return AssetType::MeshAsset; }
		static bool IsKnownSourceExtension(const std::filesystem::path& extension);
		static std::string GetSourceExtensionAlias() { return "Mesh Source"; }
		static std::string GetProxyExtension() { return ".femesh"; }
		static std::string GetProxyExtensionAlias() { return "Mesh"; }

		void SendDataToGPU(GDIType GDI, void* data);

		virtual void UnloadFromGPU() override;
		virtual void UnloadFromCPU() override;

		const ACMeshSpecification* GetSpecification() const { return GetIfExist<ACMeshSpecification>(); }
		ACMeshSpecification& GetOrEmplaceSpecification() { return GetOrEmplace<ACMeshSpecification>(); }

		void Draw(AssetHandle<MaterialInstance> materialInstance);
		void Draw() { Draw(Get<ACMaterialInstance>().MaterialInstance); }

	protected:
		Mesh(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) {}
	private:
		friend class ModelImporter;
	};

	class MeshBuilder
	{
	public:
		MeshBuilder& SetData(MeshData* data)
		{
			m_Data = data;
			m_Specification.IndicesCount = (uint32_t)data->Indices.size();
			m_Specification.VertexCount = (uint32_t)data->Vertices.size();
			return *this;
		}
		MeshBuilder& SetAssimpMaterialIndex(uint32_t assimpMaterialIndex) { m_Specification.AssimpMaterialIndex = assimpMaterialIndex; return *this; }
		MeshBuilder& AddAssimpMeshIndex(uint32_t assimpMeshIndex) { m_Specification.AssimpMeshesIndexes.push_back(assimpMeshIndex); return *this; }
		MeshBuilder& SetMaterialInstance(AssetHandle<MaterialInstance> materialInstance) { m_MaterialInstance = materialInstance; }

		void Create(AssetUser<Mesh>& textureUser);
	private:
		ACMeshSpecification m_Specification;
		MeshData* m_Data;

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