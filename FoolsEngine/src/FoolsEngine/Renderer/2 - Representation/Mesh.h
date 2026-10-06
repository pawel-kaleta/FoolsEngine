#pragma once

#include "Material.h"

#include "FoolsEngine/Foundation/Memory/Splice.h"

#include "FoolsEngine/Foundation/Utils/Core.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include <glm/glm.hpp>

namespace YAML { class Emitter; class Node; }

namespace fe::Render::Representation
{
	struct alignas(16) Vertex
	{
		GAPI::std140_float Position_x;
		GAPI::std140_float Position_y;
		GAPI::std140_float Position_z;
		GAPI::std140_float Normal_x;
		GAPI::std140_float Normal_y;
		GAPI::std140_float Normal_z;
		GAPI::std140_float Tangent_x;
		GAPI::std140_float Tangent_y;
		GAPI::std140_float Tangent_z;
		GAPI::std140_float UV_x;
		GAPI::std140_float UV_y;

		GAPI::std140_float Padding;
	};

	struct alignas(16) VertexCPU
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec2 UV0;
	};

	struct ACMesh_Core final : public AssetComponent
	{
		U32 mVertexCount;
		U32 mIndexCount;
		
		void Init()
		{
			mVertexCount = 0;
			mIndexCount = 0;
		}

		UInt DataSize() const
		{
			UInt result = 0;
			result += mVertexCount * sizeof(Vertex);
			result += AlignOffsetTo<16>(mIndexCount * sizeof(U32));
			return result;
		}
	};

	template <GAPI::Platform::ValueType tPlatform>
	struct ACMesh_RegionGPU final : public AssetComponent
	{
		GAPI::GID mBuffer;
		U32 mBufferOffset; //vertices first
		void* mRegion;
	};

	class MeshObserver : public AssetInterface
	{
	public:
		const ACMesh_Core& GetCore() const { return Get<ACMesh_Core>(); }

		template <GAPI::Platform::ValueType tPlatform>
		const ACMesh_RegionGPU<tPlatform>* GetRegionGPU() { return GetIfExist<ACMesh_RegionGPU<tPlatform>>(); }

		void Draw(const AssetObserver<Material>& materialObserver) const;
	protected:
		MeshObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class MeshUser : public MeshObserver
	{
	public:
		ACMesh_Core& GetCore() const { return Get<ACMesh_Core>(); }

		template <GAPI::Platform::ValueType tPlatform>
		ACMesh_RegionGPU<tPlatform>* GetRegionGPU() { return GetIfExist<ACMesh_RegionGPU<tPlatform>>(); }

		template <GAPI::Platform::ValueType tPlatform>
		ACMesh_RegionGPU<tPlatform>& EmplaceRegionGPU() { return Emplace<ACMesh_RegionGPU<tPlatform>>().Init(); }

		template <GAPI::Platform::ValueType tPlatform>
		void RemoveRegionGPU() { Erase<ACMesh_RegionGPU<tPlatform>>(); }

	protected:
		MeshUser(ECS_AssetHandle ECS_handle) : MeshObserver(ECS_handle) { }
	};

	class Mesh : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Mesh; }
		static constexpr const char* GetMetaFileExtension() { return ".femesh"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACMesh_Core>(assetID).Init(); }

		using Observer = MeshObserver;
		using User = MeshUser;
		using Core = ACMesh_Core;
	};
}