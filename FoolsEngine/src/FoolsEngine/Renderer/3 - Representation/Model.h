#pragma once

#include "FoolsEngine\Assets\AssetHandle.h"

#include "Mesh.h"

namespace fe
{
	enum class SkeletonType
	{
		None = 0,
		Humanoid
	};

	struct ACModelSpecification : public AssetComponent
	{
		uint32_t RenderMeshCount;

		void Init()
		{
			RenderMeshCount = 0;
		}
	};

	//struct ACModelLoadingSettings : public AssetComponent
	//{
	//	int dummy;
	//};

	struct RenderMesh
	{
		AssetID MeshID;
		AssetID MaterialInstanceID;

		void Init()
		{
			MeshID = NullAssetID;
			MaterialInstanceID = NullAssetID;
		}
	};

	struct ACRenderMeshes : public AssetComponent
	{
		std::vector<RenderMesh> BaseRenderMeshes;
	};

	class Model : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static AssetType GetTypeStatic() { return AssetType::ModelAsset; }

		virtual void PlaceCoreComponents() final override;
		virtual void Release() final override;
		void SendDataToGPU(GDIType GDI, void* data);
		void UnloadFromCPU();

		const ACModelSpecification& GetSpecification() const { return Get<ACModelSpecification>(); }
		      ACModelSpecification& GetSpecification()       { return Get<ACModelSpecification>(); }

		//const ACModelLoadingSettings* GetImportSettings() const { return GetIfExist<ACModelLoadingSettings>(); }
		//ACModelLoadingSettings& GetOrEmplaceImportSettings() { return GetOrEmplace<ACModelLoadingSettings>(); }
		
	protected:
		Model(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) {}
	private:
		friend class ModelImporter;
	};
	
}