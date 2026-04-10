#pragma once

#include "Material.h"

#include "FoolsEngine/Foundation/Utils/Core.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/2 - Resource/RMeshBindings.h"

#include <glm/glm.hpp>

namespace YAML { class Emitter; class Node; }

namespace fe
{
	struct ACMeshCore final : public AssetComponent
	{
		U32 VertexCount;
		U32 IndexCount;
		Splice<Byte> Data;
		const Description::Buffer::Layout& VertexLayout() { return Description::Buffer::Vertex::GetLayout(); };
		using Vert = Description::Buffer::Vertex;

		void Init();
		~ACMeshCore();

		Splice<U32> GetIndexBuffer()
		{
			Splice<U32> result;
			result.Elements = (U32*)Data.Elements;
			result.Count = IndexCount;
			return result;
		}
		Splice<Vert> GetVertexBuffer()
		{
			Splice<Vert> result;
			result.Elements = (Vert*)(Data.Elements + sizeof(U32) * IndexCount);
			result.Count = VertexCount;
			return result;
		}
		UInt DataSize() const { return (IndexCount * sizeof(U32)) + (VertexCount * sizeof(Vert)); }
	};

	struct ACRMeshBindings_OpenGL final : public AssetComponent
	{
		Resource::RMeshBindings_OpenGL MeshBindings;
	};

	class MeshObserver : public AssetInterface
	{
	public:
		const ACMeshCore& GetCore() const { return Get<ACMeshCore>(); }

		const ACGPUBuffer_OpenGL* GetBuffer() const { return GetIfExist<ACGPUBuffer_OpenGL>(); }

		const ACRMeshBindings_OpenGL* GetMeshBindings() const { return GetIfExist<ACRMeshBindings_OpenGL>(); }

		UInt GetGPUDataSize() const { return Get<ACMeshCore>().DataSize(); }

		void Draw(const AssetObserver<Material>& materialObserver) const;
	protected:
		MeshObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class MeshUser : public MeshObserver
	{
	public:
		ACMeshCore& GetCore() const { return Get<ACMeshCore>(); }

		ACRMeshBindings_OpenGL* GetVertexArray() const { return GetIfExist<ACRMeshBindings_OpenGL>(); }
		
		void Release() const;

		bool SendDataToGPU(GAPIType GAPI) const;
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