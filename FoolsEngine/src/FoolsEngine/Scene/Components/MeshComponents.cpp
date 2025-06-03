#include "FE_pch.h"
#include "MeshComponents.h"

#include "FoolsEngine\Scene\BaseEntity.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

namespace fe
{
	void CRenderMesh::DrawInspectorWidget(BaseEntity entity)
	{
		std::string name_tag = RenderMesh.Observe().GetFilepath().filename().string();
		DrawAssetHandle<fe::RenderMesh>(RenderMesh, name_tag);
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
		std::string material_name_tag = Material.Observe().GetFilepath().filename().string();
		std::string     mesh_name_tag = Mesh.Observe().GetFilepath().filename().string();
		DrawAssetHandle<fe::Material>(Material, mesh_name_tag);
		DrawAssetHandle<fe::Mesh    >(Mesh, mesh_name_tag);
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
		std::string name_tag = Model.Observe().GetFilepath().filename().string();
		DrawAssetHandle<fe::Model>(Model, name_tag);
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
			std::string name_tag = render_mesh.Observe().GetFilepath().filename().string();
			DrawAssetHandle<RenderMesh>(render_mesh, name_tag);
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
		for (auto& render_mesh : data["RenderMeshes"])
		{
			RenderMeshes.emplace_back() = render_mesh.as<AssetHandle<RenderMesh>>();
		}
	}
}