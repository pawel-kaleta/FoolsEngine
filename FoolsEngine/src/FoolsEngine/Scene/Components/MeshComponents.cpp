#include "FE_pch.h"
#include "MeshComponents.h"

#include "FoolsEngine\Scene\BaseEntity.h"
#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Renderer\7 - Integration\Renderer.h"
#include "FoolsEngine\Memory\Scratchpad.h"

namespace fe
{
	void CRenderMesh::DrawInspectorWidget(BaseEntity entity)
	{
		DrawAssetHandle<fe::RenderMesh>(RenderMesh);
	}

	void CRenderMesh::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "RenderMesh" << YAML::Value << RenderMesh;
	}

	void CRenderMesh::Deserialize(YAML::Node& data)
	{
		RenderMesh = data["RenderMesh"].as<AssetHandle<fe::RenderMesh>>();
	}

	void CRenderMeshView::DrawInspectorWidget(BaseEntity entity)
	{
		DrawAssetHandle<fe::Material>(Material);
		DrawAssetHandle<fe::Mesh>(Mesh);
	}

	void CRenderMeshView::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Material" << YAML::Value << Material;
		emitter << YAML::Key << "Mesh" << YAML::Value << Mesh;
	}

	void CRenderMeshView::Deserialize(YAML::Node& data)
	{
		Material = data["Material"].as<AssetHandle<fe::Material>>();
		Mesh = data["Mesh"].as<AssetHandle<fe::Mesh    >>();
	}

	void CModel::DrawInspectorWidget(BaseEntity entity)
	{
		DrawAssetHandle<fe::Model>(Model);
	}

	void CModel::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Model" << YAML::Value << Model;
	}

	void CModel::Deserialize(YAML::Node& data)
	{
		Model = data["Model"].as<AssetHandle<fe::Model>>();
	}

	void CModelView::DrawInspectorWidget(BaseEntity entity)
	{
		for (auto& render_mesh : RenderMeshes)
		{
			DrawAssetHandle<RenderMesh>(render_mesh);
		}
	}

	void CModelView::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "RenderMeshes" << YAML::Value << YAML::BeginSeq;

		for (const auto& render_mesh : RenderMeshes)
		{
			emitter << render_mesh;
		}

		emitter << YAML::EndSeq;
	}

	void CModelView::Deserialize(YAML::Node& data)
	{
		for (auto render_mesh : data["RenderMeshes"])
		{
			RenderMeshes.emplace_back() = render_mesh.as<AssetHandle<RenderMesh>>();
		}
	}
}