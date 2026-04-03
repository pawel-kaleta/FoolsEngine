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

		bool SendDataToGPU(AssetUser<Shader		 >& assetUser);
		bool SendDataToGPU(AssetUser<ShadingModel>& assetUser);
		bool SendDataToGPU(AssetUser<Texture2D	 >& assetUser);
		bool SendDataToGPU(AssetUser<Material	 >& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset);
		bool SendDataToGPU(AssetUser<Mesh		 >& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset);
		bool SendDataToGPU(AssetUser<RenderMesh	 >& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset);
		bool SendDataToGPU(AssetUser<Model		 >& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset);

		bool ReleaseDataFromGPU(AssetUser<Shader      >& assetUser);
		bool ReleaseDataFromGPU(AssetUser<ShadingModel>& assetUser);
		bool ReleaseDataFromGPU(AssetUser<Texture2D   >& assetUser);
		bool ReleaseDataFromGPU(AssetUser<Material    >& assetUser);
		bool ReleaseDataFromGPU(AssetUser<Mesh        >& assetUser);
		bool ReleaseDataFromGPU(AssetUser<RenderMesh  >& assetUser);
		bool ReleaseDataFromGPU(AssetUser<Model       >& assetUser);
	};
}