#pragma once

#include "FoolsEngine/Assets/AssetHandle.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/2 - Representation/Mesh.h"
#include "FoolsEngine/Renderer/2 - Representation/Shader.h"
#include "FoolsEngine/Renderer/2 - Representation/Texture.h"
#include "FoolsEngine/Renderer/2 - Representation/Material.h"
#include "FoolsEngine/Renderer/2 - Representation/RenderMesh.h"
#include "FoolsEngine/Renderer/2 - Representation/ShadingModel.h"

namespace fe::Render::Integration
{
	struct BaseAssetsStruct
	{
		struct {
			AssetHandle<Texture2D> Default;
			AssetHandle<Texture2D> FlatWhite;
			AssetHandle<Texture2D> FlatBlack;
		} Textures;

		struct {
			AssetHandle <Shader> Base2DVertex;
			AssetHandle <Shader> Base2DFragmentOpaque;
			AssetHandle <Shader> Base2DFragmentCutout;
			AssetHandle <Shader> Base2DFragmentBlend;
			AssetHandle <Shader> Base3DVertex;
			AssetHandle <Shader> Base3DFragmentOpaque;
			AssetHandle <Shader> Base3DFragmentCutout;
			AssetHandle <Shader> Base3DFragmentBlend;
		} Shaders;

		struct {
			AssetHandle<ShadingModel> Base2DFlatOpaque;
			AssetHandle<ShadingModel> Base2DFlatCutout;
			AssetHandle<ShadingModel> Base2DFlatBlend;
			AssetHandle<ShadingModel> Base3DOpaque;
			AssetHandle<ShadingModel> Base3DCutout;
			AssetHandle<ShadingModel> Base3DBlend;
		} ShadingModels;

		struct {
			AssetHandle<Material> Default2DFlatOpaque; // do I need this material?
			AssetHandle<Material> Default2DFlatCutout; // do I need this material?
			AssetHandle<Material> Default2DFlatBlend; // do I need this material?
			AssetHandle<Material> DefaultOpaque;
			AssetHandle<Material> DefaultCutout;
			AssetHandle<Material> DefaultTranslucent;
		} Materials;
	};
}