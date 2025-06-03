#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\VertexData.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\IndexBuffer.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"

#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"
#include "FoolsEngine\Assets\AssetHandle.h"

#include "FoolsEngine\Core\Core.h"

#include <glm\glm.hpp>

namespace fe
{
	struct MeshSpecification
	{
		uint32_t VertexCount;
		uint32_t IndexCount;
		VertexData::Layout VertexLayout() { return VertexData::Vertex::GetLayout(); };

		void Init()
		{
			VertexCount = 0;
			IndexCount = 0;
		}
	};

	struct ACMeshCore final : public AssetComponent
	{
		MeshSpecification Specification;
		void* Data; // allocated as float[ DataSize()/sizeof(float) ]

		void Init();
		~ACMeshCore();

		uint32_t* GetIndexArrayPtr() { return (uint32_t*)Data; }
		float* GetVertexArrayPtr() { return (float*)((uint32_t*)Data + Specification.IndexCount); }
		size_t DataSize() { return (Specification.IndexCount * sizeof(uint32_t)) + (Specification.VertexCount * sizeof(VertexData::Vertex)); }
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
		const ACMeshCore& GetCoreComponent() const { return Get<ACMeshCore>(); }

		const ACGPUBuffers* GetBuffers() const { return GetIfExist<ACGPUBuffers>(); }
		//const ACMeshLoadingSettings* GetImportSettings() const { return GetIfExist<ACMeshLoadingSettings>(); }

		void Draw(const AssetObserver<Material>& materialObserver) const;
	protected:
		MeshObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class MeshUser : public MeshObserver
	{
	public:
		ACMeshCore& GetCoreComponent() const { return Get<ACMeshCore>(); }

		ACGPUBuffers* GetBuffers() const { return GetIfExist<ACGPUBuffers>(); }
		//ACMeshLoadingSettings& GetOrEmplaceImportSettings() const { return GetOrEmplace<ACMeshLoadingSettings>(); }
		
		void Release() const;

		void SendDataToGPU(GDIType GDI) const;
		void UnloadFromCPU() const;

	protected:
		MeshUser(ECS_AssetHandle ECS_handle) : MeshObserver(ECS_handle) {}
	};

	class Mesh : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::MeshAsset; }
		static void EmplaceCore(AssetID assetID) { AssetManager::GetRegistry().emplace<ACMeshCore>(assetID).Init(); }
		static void Serialize(const AssetObserver<Mesh>& assetObserver);
		static bool Deserialize(AssetID assetID);

		using Observer = MeshObserver;
		using User = MeshUser;
		using Core = ACMeshCore;
	};
}