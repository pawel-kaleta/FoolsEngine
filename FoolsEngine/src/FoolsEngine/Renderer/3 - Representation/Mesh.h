#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\VertexData.h"

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\IndexBuffer.h"

#include "FoolsEngine\Renderer\3 - Representation\ShadingModel.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"

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
		glm::vec3 Tangent;
		glm::vec2 UV0;
		glm::vec2 UV1;

		static VertexData::Layout GetLayout() {
			return VertexData::Layout({
				{ ShaderData::Type::Float3, "a_Position" },
				{ ShaderData::Type::Float3, "a_Normal" },
				{ ShaderData::Type::Float3, "a_Tangent" },
				{ ShaderData::Type::Float2, "a_UV0" },
				{ ShaderData::Type::Float2, "a_UV1" }
			});
		}
	};

	struct MeshSpecification
	{
		uint32_t VertexCount;
		uint32_t IndexCount;
		VertexData::Layout VertexLayout() { return Vertex::GetLayout(); };

		void Init()
		{
			VertexCount = 0;
			IndexCount = 0;
		}
	};

	struct ACMeshData final : public AssetComponent
	{
		MeshSpecification Specification;
		void* Data; // allocated as float[ DataSize()/sizeof(float) ]

		void Init();
		~ACMeshData();

		uint32_t* GetIndexArrayPtr() { return (uint32_t*)Data; }
		float* GetVertexArrayPtr() { return (float*)((uint32_t*)Data + Specification.IndexCount); }
		size_t DataSize() { return (Specification.IndexCount * sizeof(uint32_t)) + (Specification.VertexCount * sizeof(Vertex)); }
	};

	//struct ACMeshLoadingSettings final : public AssetComponent
	//{
	//	// -1 means aiProcess_PreTransformVertices and merge as one mesh
	//	uint32_t AssimpMeshIndex = -1; 
	//	//uint32_t AssimpImportFlags = 0; // aiPostProcessSteps
	//};

	struct ACGPUBuffers final : public AssetComponent
	{
		Ref<VertexBuffer> VertexBuffer;
		Ref<IndexBuffer>  IndexBuffer;
	};

	class MeshObserver : public AssetInterface
	{
	public:
		virtual AssetType GetType() const override final { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::MeshAsset; }

		const ACGPUBuffers* GetBuffers() const { return GetIfExist<ACGPUBuffers>(); }
		const MeshSpecification& GetSpecification() const { return Get<ACMeshData>().Specification; }
		//const ACMeshLoadingSettings* GetImportSettings() const { return GetIfExist<ACMeshLoadingSettings>(); }

		void Draw(const AssetObserver<Material>& materialObserver) const;
	protected:
		MeshObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class MeshUser : public MeshObserver
	{
	public:
		void PlaceCoreComponent() const { Emplace<ACMeshData>().Init(); };
		void Release() const;

		void SendDataToGPU(GDIType GDI) const;
		void UnloadFromCPU() const;

		ACGPUBuffers* GetBuffers() const { return GetIfExist<ACGPUBuffers>(); }
		MeshSpecification& GetSpecification() const { return Get<ACMeshData>().Specification; }

		//ACMeshLoadingSettings& GetOrEmplaceImportSettings() const { return GetOrEmplace<ACMeshLoadingSettings>(); }

	protected:
		MeshUser(ECS_AssetHandle ECS_handle) : MeshObserver(ECS_handle) {}
	};

	class Mesh : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::MeshAsset; }

		using Observer = MeshObserver;
		using User = MeshUser;
	};
}