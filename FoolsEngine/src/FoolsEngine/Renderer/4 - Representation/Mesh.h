#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\2 - Resource\VertexArray.h"
#include "Material.h"

#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"
#include "FoolsEngine\Assets\AssetHandle.h"

#include "FoolsEngine\Core\Core.h"

#include <glm\glm.hpp>

namespace YAML { class Emitter; class Node; }

namespace fe
{
	struct MeshSpecification
	{
		uint32_t VertexCount;
		uint32_t IndexCount;
		const Description::Buffer::Layout& VertexLayout() { return Description::Buffer::Vertex::GetLayout(); };

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

		uint32_t* GetIndexBufferPtr() { return (uint32_t*)Data; }
		float* GetVertexBufferPtr() { return (float*)((uint32_t*)Data + Specification.IndexCount); }
		size_t DataSize() { return (Specification.IndexCount * sizeof(uint32_t)) + (Specification.VertexCount * sizeof(Description::Buffer::Vertex)); }
	};

	struct ACGPUVertexArray
	{
		Resource::VertexArray_OpenGL VertexArray;
	};

	class MeshObserver : public AssetInterface
	{
	public:
		const ACMeshCore& GetCoreComponent() const { return Get<ACMeshCore>(); }

		const ACGPUBuffer* GetBuffer() const { return GetIfExist<ACGPUBuffer>(); }

		const ACGPUVertexArray* GetVertexArray() const { return GetIfExist<ACGPUVertexArray>(); }

		size_t GetDataSize() const { return Get<ACMeshCore>().DataSize(); }

		void Draw(const AssetObserver<Material>& materialObserver) const;
	protected:
		MeshObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class MeshUser : public MeshObserver
	{
	public:
		ACMeshCore& GetCoreComponent() const { return Get<ACMeshCore>(); }

		ACGPUBuffer* GetBuffer() const { return GetIfExist<ACGPUBuffer>(); }
		ACGPUVertexArray* GetVertexArray() const { return GetIfExist<ACGPUVertexArray>(); }
		
		void Release() const;

		bool SendDataToGPU(GAPIType GAPI) const;
		bool SendDataToGPUInternal(GAPIType GAPI, Resource::StaticBufferBase* buffer, uint32_t offset) const;
		void UnloadFromCPU() const;

	protected:
		MeshUser(ECS_AssetHandle ECS_handle) : MeshObserver(ECS_handle) {}
	};

	class Mesh : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Mesh; }
		static constexpr const char* GetMetaFileExtension() { return ".femesh"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACMeshCore>(assetID).Init(); }
		static void SaveMetadata(YAML::Emitter& emitter, AssetID assetID);
		static bool LoadMetadata(AssetID assetID);
		static AssetID LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath);

		using Observer = MeshObserver;
		using User = MeshUser;
		using Core = ACMeshCore;
	};
}