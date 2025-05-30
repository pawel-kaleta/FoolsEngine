#include "FE_pch.h"
#include "AssetTypesRegistry.h"

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\3 - Representation\ShadingModel.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"
#include "FoolsEngine\Renderer\3 - Representation\RenderMesh.h"
#include "FoolsEngine\Renderer\3 - Representation\Model.h"
#include "FoolsEngine\Scene\Scene.h"

namespace fe
{
	void AssetTypesRegistry::RegisterAssetTypes()
	{
		RegisterAssetType<Texture2D>();
		RegisterAssetType<Shader>();
		RegisterAssetType<ShadingModel>();
		RegisterAssetType<Material>();
		RegisterAssetType<Mesh>();
		RegisterAssetType<RenderMesh>();
		RegisterAssetType<Model>();
		RegisterAssetType<Scene>();
	}
}