#pragma once

#include "FoolsEngine/Scene/Component.h"

#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"
#include "FoolsEngine\Renderer\3 - Representation\RenderMesh.h"
#include "FoolsEngine\Renderer\3 - Representation\Model.h"

namespace fe
{
	struct CRenderMesh final : SpatialComponent
	{
		FE_COMPONENT_SETUP(CRenderMesh, "RenderMesh");

		AssetHandle<RenderMesh> RenderMesh;

		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};

	struct CRenderMeshView final : DataComponent
	{
		FE_COMPONENT_SETUP(CRenderMeshView, "RenderMeshView");

		AssetHandle<Material> Material;
		AssetHandle<Mesh> Mesh;

		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};

	struct CModel final : SpatialComponent
	{
		FE_COMPONENT_SETUP(CModel, "Model");

		AssetHandle<Model> Model;

		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};

	struct CModelView final : SpatialComponent
	{
		FE_COMPONENT_SETUP(CModelView, "ModelView");

		std::vector<AssetHandle<RenderMesh>> RenderMeshes;

		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};
}