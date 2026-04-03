#pragma once

#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

#include "FoolsEngine/Assets/AssetTypes.h"

namespace fe
{
	struct ResourceManager { };

	struct ResourceManagerOpenGL final : public ResourceManager
	{
		TypedAlloc<MallocAlloc> DefaultAlloc;
		TypedAlloc<MallocAlloc> AuxilaryAlloc;
		TypedAlloc<MonotonicAlloc> PermanentAlloc;

		void Create() { PermanentAlloc.Init(); }

		bool SendProjectAssetToGPU() {};

		bool SendDataToGPU(AssetUser<Shader		 >& assetUser);
		bool SendDataToGPU(AssetUser<ShadingModel>& assetUser);
		bool SendDataToGPU(AssetUser<Texture2D	 >& assetUser);
		bool SendDataToGPU(AssetUser<Material	 >& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset);
		bool SendDataToGPU(AssetUser<Mesh		 >& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset);
		bool SendDataToGPU(AssetUser<RenderMesh	 >& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset);
		bool SendDataToGPU(AssetUser<Model		 >& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset);

		void ReleaseDataFromGPU(AssetUser<Shader      >& assetUser);
		void ReleaseDataFromGPU(AssetUser<ShadingModel>& assetUser);
		void ReleaseDataFromGPU(AssetUser<Texture2D   >& assetUser);
		void ReleaseDataFromGPU(AssetUser<Material    >& assetUser);
		void ReleaseDataFromGPU(AssetUser<Mesh        >& assetUser);
		void ReleaseDataFromGPU(AssetUser<RenderMesh  >& assetUser);
		void ReleaseDataFromGPU(AssetUser<Model       >& assetUser);

		class ContextScope
		{
		public:
			ContextScope(ResourceManagerOpenGL* rm) :
				DefaultAlloc(Context::Allocators::Default),
				AuxilaryAlloc(Context::Allocators::Auxiliary)
			{
				Context::Allocators::Default = (TypedAlloc<Allocator>*) & rm->DefaultAlloc;
				Context::Allocators::Auxiliary = (TypedAlloc<Allocator>*) & rm->AuxilaryAlloc;
			}
			~ContextScope()
			{
				Context::Allocators::Default = DefaultAlloc;
				Context::Allocators::Auxiliary = AuxilaryAlloc;
			}
		private:
			TypedAlloc<Allocator>* DefaultAlloc;
			TypedAlloc<Allocator>* AuxilaryAlloc;
		};
	};
}