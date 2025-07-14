#include "FE_pch.h"
#include "MeshComponents.h"

#include "FoolsEngine\Scene\BaseEntity.h"
#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "FoolsEngine\Memory\Scratchpad.h"

namespace fe
{
	void EditMaterial(const AssetHandle<Material>& material)
	{
		if (!material.IsValid())
		{
			return;
		}

		Scratchpad sp;

		auto material_user = material.Use();
		auto& shading_model_current = material_user.GetCoreComponent().ShadingModelHandle;

		if (ImGui::BeginCombo("Shading Model", shading_model_current.Observe().GetFilepath().filename().string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str()))
		{
			bool is_selected;

			auto shading_models = AssetManager::GetRegistry().view<ACShadingModelCore>();

			for (auto id : shading_models)
			{
				auto shading_model_observer = AssetObserver<ShadingModel>(id);
				is_selected = (shading_model_current.GetID() == id);

				if (ImGui::Selectable(shading_model_observer.GetFilepath().filename().string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str(), is_selected))
				{
					material_user.MakeMaterial(shading_model_observer);
					shading_model_current.SetID(id);
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		auto shading_model_observer = shading_model_current.Observe();

		auto& sm_core_component = shading_model_observer.GetCoreComponent();
		auto& material_core_component = material_user.GetCoreComponent();

		for (auto& uniform : sm_core_component.Uniforms)
		{
			ImGuiLayer::RenderUniform(uniform, material_user.GetUniformValuePtr(material_core_component, uniform));
		}

		for (auto& textureSlot : sm_core_component.TextureSlots)
		{
			auto texture_current = material_user.GetTextureID(material_core_component, textureSlot);
			bool newSelection = false;

			bool validID = texture_current != NullAssetID;

			const char* texture_combo_preview = !validID ? "None" : AssetObserver<Texture2D>(texture_current).GetFilepath().filename().string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str();
			if (ImGui::BeginCombo(textureSlot.GetName().c_str(), texture_combo_preview))
			{
				bool is_selected = validID;

				if (ImGui::Selectable("None", is_selected))
					material_user.SetTexture(material_core_component, textureSlot, NullAssetID);

				auto textures = AssetManager::GetRegistry().view<ACTexture2DCore, ACRefsCounters>();
				for (auto id : textures)
				{
					auto textureHandle = AssetHandle<Texture2D>(id);
					is_selected = (texture_current == textureHandle.GetID());

					if (ImGui::Selectable(textureHandle.Observe().GetFilepath().filename().string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str(), is_selected))
					{
						newSelection = true;
						texture_current = textureHandle.GetID();
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (newSelection)
			{
				material_user.SetTexture(material_core_component, textureSlot, texture_current);
			}
		}
	}

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