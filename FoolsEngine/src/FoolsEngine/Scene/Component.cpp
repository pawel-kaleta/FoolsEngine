#include "FE_pch.h"
#include "Component.h"

#include "BaseEntity.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"
#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"
#include "FoolsEngine\Renderer\3 - Representation\RenderMesh.h"
#include "FoolsEngine\Renderer\3 - Representation\Model.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

#include "FoolsEngine\Assets\Loaders\LoadersRegistry.h"
#include "FoolsEngine\Memory\Scratchpad.h"
#include "FoolsEngine\Core\Project.h"

#include <type_traits>

namespace fe
{
	void DataComponent::DrawInspectorWidget(BaseEntity entity)
	{
		FE_LOG_CORE_ERROR("UI widget drawing of {0} not implemented!", this->GetName());
	}

	void DataComponent::Serialize(YAML::Emitter& emitter)
	{
		FE_LOG_CORE_ERROR("{0} serialization not implemented!", this->GetName());
	}

	void DataComponent::Deserialize(YAML::Node& data)
	{
		FE_LOG_CORE_ERROR("{0} deserialization not implemented!", this->GetName());
	}

	void SpatialComponent::SerializeOffset(YAML::Emitter& emitter) const
	{
		emitter << YAML::Key << "Offset" << YAML::BeginMap;

		emitter << YAML::Key << "Shift"    << YAML::Value << Offset.Shift;
		emitter << YAML::Key << "Rotation" << YAML::Value << Offset.Rotation;
		emitter << YAML::Key << "Scale"    << YAML::Value << Offset.Scale;

		emitter << YAML::EndMap;
	}

	void SpatialComponent::DeserializeOffset(YAML::Node& data)
	{
		auto node = data["Offset"];
		Offset.Shift = node["Shift"].as<glm::vec3>();
		Offset.Rotation = node["Rotation"].as<glm::vec3>();
		Offset.Scale = node["Scale"].as<glm::vec3>();
	}

	template<typename tnAsset>
	void DataComponent::DrawAssetHandle(AssetHandle<tnAsset>& assetHandle, const std::pmr::string& nameTag, AssetID defaultAsset)
	{
		Scratchpad sp;

		ImGui::SeparatorText(nameTag.c_str());

		std::pmr::string name(&sp);
		if (!assetHandle.IsValid())
		{
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.25f,0.25f,0.25f,1.0f });
			name = "-";
		}
		else
		{
			auto observer = assetHandle.Observe();
			name = std::to_string(assetHandle.GetID());
			name += ": ";
			if (observer.AllOf<ACFilepath>())
				name += observer.GetFilepath().filename().string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
			else
				name += "default";
		}
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
		bool reset = ImGui::Button("x"); ImGui::SameLine();
		bool selected = ImGui::Button(name.c_str(), { ImGui::GetContentRegionAvail().x / 2, 0 }); ImGui::SameLine();

#ifdef FE_EDITOR
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetPath"))
			{
				IM_ASSERT(payload->DataSize == sizeof(std::filesystem::path));
				const std::filesystem::path& filepath = *(const std::filesystem::path*)payload->Data;
				if (!filepath.empty())
				{
					std::pmr::string extension = filepath.extension().string<PMR_STRING_TEMPLATE_PARAMS>(&sp);

					if (tnAsset::GetMetaFileExtension() == extension)
					{
						AssetID assetID = AssetManager::GetAssetFromFilepath(filepath.lexically_relative(Project::GetInstance()->AssetsPath));
						if (assetID != NullAssetID)
						{
							assetHandle.SetID(assetID);
						}
						else
						{
							FE_CORE_ASSERT(false, "This asset was imported to a different project!");
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
#endif // FE_EDITOR

		ImGui::Text("Asset");

		auto loading_priority = assetHandle.GetLoadingPriority();
		ImGui::Button(loading_priority.ToConstCharPtr(), {ImGui::GetContentRegionAvail().x / 2, 0}); ImGui::SameLine();
		if (loading_priority == AssetLoadingPriority::None)
		{
			ImGui::BeginDisabled();
			ImGui::Button("-");
			ImGui::EndDisabled();
		}
		else
		{
			if (ImGui::Button("-"))
			{
				loading_priority.FromInt(loading_priority.ToInt()-1);
				assetHandle.SetLoadingPriority(loading_priority);
			}
		}

		ImGui::SameLine();

		if (loading_priority == AssetLoadingPriority::Critical)
		{
			ImGui::BeginDisabled();
			ImGui::Button("+");
			ImGui::EndDisabled();
		}
		else
		{
			if (ImGui::Button("+"))
			{
				loading_priority.FromInt(loading_priority.ToInt()+1);
				assetHandle.SetLoadingPriority(loading_priority);
			}
		}

		ImGui::SameLine();
		ImGui::Text("Loading Priority");

		ImGui::PopStyleVar();
		
		if (!assetHandle.IsValid())
			ImGui::PopStyleColor();

		if (reset)
			assetHandle.SetID(defaultAsset);

		ImGui::Separator();
	}
	template void DataComponent::DrawAssetHandle<Texture2D   >(AssetHandle<Texture2D   >&, const std::pmr::string&, AssetID);
	template void DataComponent::DrawAssetHandle<Material    >(AssetHandle<Material    >&, const std::pmr::string&, AssetID);
	template void DataComponent::DrawAssetHandle<Mesh        >(AssetHandle<Mesh        >&, const std::pmr::string&, AssetID);
	template void DataComponent::DrawAssetHandle<RenderMesh  >(AssetHandle<RenderMesh  >&, const std::pmr::string&, AssetID);
	template void DataComponent::DrawAssetHandle<Model       >(AssetHandle<Model       >&, const std::pmr::string&, AssetID);


	
	//---------------------------------------------------------
	// this should not be here

	void EditMaterial()
	{
		AssetHandle<Material> material;

		if (!material.IsValid())
		{
			return;
		}

		auto material_user = material.Use();
		auto& shading_model_current = material_user.GetCoreComponent().ShadingModelHandle;
		Scratchpad sp;
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
			
			bool  validID = texture_current != NullAssetID;
			const char* texture_combo_preview = !validID ? "None" : AssetObserver<Texture2D>(texture_current).GetFilepath().filename().string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str();
			if (ImGui::BeginCombo(textureSlot.GetName().c_str(), texture_combo_preview))
			{
				bool is_selected = validID;

				if (ImGui::Selectable("None", is_selected))
					material_user.SetTexture(material_core_component, textureSlot, NullAssetID);

				auto textures = AssetManager::GetRegistry().view<ACTexture2DCore>();
				for (auto id : textures)
				{
					auto textureHandle = AssetHandle<Texture2D>(id);
					is_selected = (texture_current == textureHandle.GetID());

					if (ImGui::Selectable(textureHandle.Observe().GetFilepath().filename().string().c_str(), is_selected))
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
}