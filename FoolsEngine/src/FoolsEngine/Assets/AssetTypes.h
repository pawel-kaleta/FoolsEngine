#pragma once

#include "FoolsEngine/Scene/Scene.h"

#include "FoolsEngine/Renderer/5 - Representation/Texture.h"
#include "FoolsEngine/Renderer/5 - Representation/Shader.h"
#include "FoolsEngine/Renderer/5 - Representation/ShadingModel.h"
#include "FoolsEngine/Renderer/5 - Representation/Material.h"
#include "FoolsEngine/Renderer/5 - Representation/Mesh.h"
#include "FoolsEngine/Renderer/5 - Representation/RenderMesh.h"
#include "FoolsEngine/Renderer/5 - Representation/Model.h"

namespace fe
{
#define FE_ASSET_TYPES_LIST Texture2D, Shader, ShadingModel, Material, Mesh, RenderMesh, Model, Scene

#define _ASSET_CONCEPT_VERIFICATION(x) static_assert(AssetConcept<x>);
	FE_FOR_EACH(_ASSET_CONCEPT_VERIFICATION, FE_ASSET_TYPES_LIST);
}