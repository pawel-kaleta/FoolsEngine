#pragma once

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
#define FE_ASSET_TYPES_LIST Texture2D, Shader, ShadingModel, Material, Mesh, RenderMesh, Model, Scene
}