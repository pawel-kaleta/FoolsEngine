#include "FE_pch.h"

#include "AssetLibrary.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"

namespace fe
{
	std::unordered_map<AssetID, AssetHandleTracker> AssetLibrary::s_Maps[AssetType::TypesCount];
}