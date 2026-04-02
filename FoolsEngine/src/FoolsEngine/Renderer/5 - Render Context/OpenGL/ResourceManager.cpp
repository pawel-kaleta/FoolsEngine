#include "FE_pch.h"

#include "FoolsEngine/Renderer/5 - Render Context/ResourceManager.h"

#include "FoolsEngine/Renderer/4 - Representation/Mesh.h"

namespace fe
{
	void ResourceManagerOpenGL::SendDataToGPU(AssetUser<Mesh        >& assetUser){}
	void ResourceManagerOpenGL::SendDataToGPU(AssetUser<ShadingModel>& assetUser){}
	void ResourceManagerOpenGL::SendDataToGPU(AssetUser<Texture2D	>& assetUser){}
	void ResourceManagerOpenGL::SendDataToGPU(AssetUser<Material	>& assetUser){}
	void ResourceManagerOpenGL::SendDataToGPU(AssetUser<Mesh		>& assetUser){}
	void ResourceManagerOpenGL::SendDataToGPU(AssetUser<RenderMesh	>& assetUser){}
	void ResourceManagerOpenGL::SendDataToGPU(AssetUser<Model		>& assetUser){}

	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<Shader      >& assetUser){}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<ShadingModel>& assetUser){}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<Texture2D   >& assetUser){}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<Material    >& assetUser){}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<Mesh        >& assetUser){}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<RenderMesh  >& assetUser){}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<Model       >& assetUser){}
}