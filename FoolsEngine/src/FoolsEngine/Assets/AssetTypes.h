#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Foundation/Utils/ForEach.h"


#include "FoolsEngine/Scene/Scene.h"

#include "FoolsEngine/Renderer/2 - Representation/Texture.h"
#include "FoolsEngine/Renderer/2 - Representation/Shader.h"
#include "FoolsEngine/Renderer/2 - Representation/ShadingModel.h"
#include "FoolsEngine/Renderer/2 - Representation/Material.h"
#include "FoolsEngine/Renderer/2 - Representation/Mesh.h"
#include "FoolsEngine/Renderer/2 - Representation/RenderMesh.h"
#include "FoolsEngine/Renderer/2 - Representation/Model.h"

namespace fe
{
#define FE_ASSET_TYPES_LIST Texture2D, Shader, ShadingModel, Material, Mesh, RenderMesh, Model, Scene

#define _ASSET_CONCEPT_VERIFICATION(x) static_assert(AssetConcept<x>);
	FE_FOR_EACH(_ASSET_CONCEPT_VERIFICATION, FE_ASSET_TYPES_LIST);
}