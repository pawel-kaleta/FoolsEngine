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

#include <type_traits>

namespace fe
{
	namespace AssetImportModal
	{
		extern void OpenWindow(const std::filesystem::path& filepath, uint32_t loaderIndex, AssetType type, AssetHandleBase* optionalBaseHandle);
	}

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
	void DataComponent::DrawAssetHandle(AssetHandle<tnAsset>& assetHandle, const std::string& nameTag)
	{
		std::string name;
		if (!assetHandle.IsValid())
		{
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.25f,0.25f,0.25f,1.0f });
			name = "-";
		}
		else
		{
			name = std::to_string(assetHandle.GetID()) + ": " + assetHandle.Observe().GetFilepath().filename().string();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FrameBorderSize, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
		bool selected = ImGui::Button(name.c_str(), { ImGui::GetContentRegionAvail().x / 2, ImGui::GetTextLineHeightWithSpacing() });
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		if (!assetHandle.IsValid())
			ImGui::PopStyleColor();

#ifdef FE_EDITOR
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetPath"))
			{
				IM_ASSERT(payload->DataSize == sizeof(std::filesystem::path));
				const std::filesystem::path& filepath = *(const std::filesystem::path*)payload->Data;
				if (!filepath.empty())
				{
					std::pmr::string extension = filepath.extension().string<PMR_STRING_TEMPLATE_PARAMS>();

					FE_CORE_ASSERTION_BREAK(false, "Not implemented");

					uint32_t file_importer_idx;// = FileHandler::GetSourceAliasAndLoaderIndex(extension, std::pmr::string());
					bool     is_asset_proxy;// = FileHandler::GetAliasAndAssetTypeOfProxy(extension, std::pmr::string()) != AssetType::None;

					if (is_asset_proxy || file_importer_idx != -1)
					{
						AssetID assetID = AssetManager::GetAssetFromFilepath(filepath);
						if (assetID != NullAssetID)
						{
							assetHandle = AssetHandle<tnAsset>(assetID);
						}
						else
						{
							FE_CORE_ASSERT(!is_asset_proxy, "Dont import asset proxies!");

							FE_CORE_ASSERTION_BREAK(false, "Not implemented");
							uint32_t file_importer_idx;// = FileHandler::GetSourceAliasAndLoaderIndex(extension, std::pmr::string());
							if (file_importer_idx != -1)
								AssetImportModal::OpenWindow(filepath, file_importer_idx, tnAsset::GetTypeStatic(), &assetHandle);
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
#endif // FE_EDITOR

		ImGui::SameLine();
		ImGui::Text(nameTag.c_str());
	}
	template void DataComponent::DrawAssetHandle<Texture2D   >(AssetHandle<Texture2D   >&, const std::string&);
	template void DataComponent::DrawAssetHandle<Material    >(AssetHandle<Material    >&, const std::string&);
	template void DataComponent::DrawAssetHandle<Mesh        >(AssetHandle<Mesh        >&, const std::string&);
	template void DataComponent::DrawAssetHandle<RenderMesh  >(AssetHandle<RenderMesh  >&, const std::string&);
	template void DataComponent::DrawAssetHandle<Model       >(AssetHandle<Model       >&, const std::string&);


	
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
		auto& shading_model_current = material_user.GetCoreComponent().ShadingModelID;

		if (ImGui::BeginCombo("Shading Model", AssetObserver<ShadingModel>(shading_model_current).GetFilepath().filename().string().c_str()))
		{
			bool is_selected;

			auto shading_models = AssetManager::GetRegistry().view<ACShadingModelCore>();

			for (auto id : shading_models)
			{
				auto shading_model_observer = AssetObserver<ShadingModel>(id);
				is_selected = (shading_model_current == id);

				if (ImGui::Selectable(shading_model_observer.GetFilepath().filename().string().c_str(), is_selected))
				{
					material_user.MakeMaterial(shading_model_observer);
					shading_model_current = id;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		auto shading_model_observer = AssetObserver<ShadingModel>(shading_model_current);

		auto& sm_core_component = shading_model_observer.GetCoreComponent();
		auto& material_core_component = material_user.GetCoreComponent();

		for (auto& uniform : sm_core_component.Uniforms)
		{
			ImGuiLayer::RenderUniform(uniform, material_user.GetUniformValuePtr(material_core_component, uniform));
		}

		for (auto& textureSlot : sm_core_component.TextureSlots)
		{
			auto texture_current = material_user.GetTexture(material_core_component, textureSlot);
			bool newSelection = false;

			const char* texture_combo_preview = !texture_current.IsValid() ? "None" : texture_current.Observe().GetFilepath().filename().string().c_str();
			if (ImGui::BeginCombo(textureSlot.GetName().c_str(), texture_combo_preview))
			{
				bool is_selected = !(texture_current.IsValid());

				if (ImGui::Selectable("None", is_selected))
					material_user.SetTexture(material_core_component, textureSlot, NullAssetID);

				auto textures = AssetManager::GetRegistry().view<ACTexture2DCore>();
				for (auto id : textures)
				{
					auto textureHandle = AssetHandle<Texture2D>(id);
					is_selected = (texture_current.GetID() == textureHandle.GetID());

					if (ImGui::Selectable(textureHandle.Observe().GetFilepath().filename().string().c_str(), is_selected))
					{
						newSelection = true;
						texture_current = textureHandle;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (newSelection)
			{
				material_user.SetTexture(material_core_component, textureSlot, texture_current.GetID());
			}
		}
	}
}