#pragma once

#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

#include "FoolsEngine/Assets/AssetTypes.h"

namespace fe
{
	struct ResourceManager
	{
		TypedAlloc<MallocAlloc> DefaultAlloc;
		TypedAlloc<MallocAlloc> AuxilaryAlloc;
		TypedAlloc<MonotonicAlloc> PermanentAlloc;

		void Create()
		{
			PermanentAlloc.Init();
		}

		class ContextScope
		{
		public:
			ContextScope(ResourceManager* rm) :
				DefaultAlloc(Context::Allocators::Default),
				AuxilaryAlloc(Context::Allocators::Auxiliary)
			{
				Context::Allocators::Default	= (TypedAlloc<Allocator>*) & rm->DefaultAlloc;
				Context::Allocators::Auxiliary	= (TypedAlloc<Allocator>*) & rm->AuxilaryAlloc;
			}
			~ContextScope()
			{
				Context::Allocators::Default = (TypedAlloc<Allocator>*) DefaultAlloc;
				Context::Allocators::Auxiliary = (TypedAlloc<Allocator>*) AuxilaryAlloc;
			}
		private:
			Allocator* DefaultAlloc;
			Allocator* AuxilaryAlloc;
		};
	};

	struct ResourceManagerOpenGL final : public ResourceManager
	{

		void SendDataToGPU(AssetUser<Shader		 >& assetUser);
		void SendDataToGPU(AssetUser<ShadingModel>& assetUser);
		void SendDataToGPU(AssetUser<Texture2D	 >& assetUser);
		void SendDataToGPU(AssetUser<Material	 >& assetUser);
		void SendDataToGPU(AssetUser<Mesh		 >& assetUser);
		void SendDataToGPU(AssetUser<RenderMesh	 >& assetUser);
		void SendDataToGPU(AssetUser<Model		 >& assetUser);

		void ReleaseDataFromGPU(AssetUser<Shader      >& assetUser);
		void ReleaseDataFromGPU(AssetUser<ShadingModel>& assetUser);
		void ReleaseDataFromGPU(AssetUser<Texture2D   >& assetUser);
		void ReleaseDataFromGPU(AssetUser<Material    >& assetUser);
		void ReleaseDataFromGPU(AssetUser<Mesh        >& assetUser);
		void ReleaseDataFromGPU(AssetUser<RenderMesh  >& assetUser);
		void ReleaseDataFromGPU(AssetUser<Model       >& assetUser);
	};
}