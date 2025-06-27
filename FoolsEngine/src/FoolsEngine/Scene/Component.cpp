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
		ImGui::TextWrapped("UI widget drawing not implemented");
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
	void DataComponent::DrawAssetHandle(AssetHandle<tnAsset>& assetHandle)
	{
		Scratchpad sp;
		ImGui::PushID((const void *) & assetHandle);
		
		auto flags =
			ImGuiChildFlags_Border |
			ImGuiChildFlags_AutoResizeY;

		ImGui::BeginChild("AssetHandleWindow", ImVec2(0, 0), flags);

		ImGui::Text(tnAsset::GetTypeStatic().ToConstCharPtr());

		ImGuiStyle& style = ImGui::GetStyle();
		const float square_button_size = ImGui::GetFrameHeight();
		const ImVec2 square_button_dimentions = { square_button_size, square_button_size };
		const float button_width__handle			= (ImGui::GetContentRegionAvail().x / 2) -		(style.ItemInnerSpacing.x + square_button_size);
		const float button_width__loading_priority	= (ImGui::GetContentRegionAvail().x / 2) - 2 *	(style.ItemInnerSpacing.x + square_button_size);
		bool handle_valid = assetHandle.IsValid();
		bool reset_handle = false;

		// asset handle
		{
			reset_handle = ImGui::Button("x", square_button_dimentions); ImGui::SameLine(0, style.ItemInnerSpacing.x);

			std::pmr::string asset_name(&sp); {
				if (!handle_valid)	asset_name = "<empty>";
				else				asset_name = assetHandle.Observe().GetFilepath().stem().string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
			}

			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
			ImGui::Button(asset_name.c_str(), { button_width__handle, 0 }); ImGui::SameLine(0, style.ItemInnerSpacing.x);
			ImGui::PopStyleVar();

			// Editor Drag Drop handling
			{
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
			}

			ImGui::Text("Asset");
		}

		// loading priority
		{
			auto loading_priority = assetHandle.GetLoadingPriority();
			
			std::pmr::string loading_priority_string(&sp); {
				loading_priority_string += std::to_string(loading_priority.ToInt());
				loading_priority_string += " - ";
				loading_priority_string += loading_priority.ToConstCharPtr();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
			ImGui::Button(loading_priority_string.c_str(), { button_width__loading_priority, 0 }); ImGui::SameLine(0, style.ItemInnerSpacing.x);
			ImGui::PopStyleVar();

			// dicrease priority
			{
				if (loading_priority == AssetLoadingPriority::None)
				{
					ImGui::BeginDisabled();
					ImGui::Button("-", square_button_dimentions);
					ImGui::EndDisabled();
				}
				else
				{
					if (ImGui::Button("-", square_button_dimentions))
					{
						loading_priority.FromInt(loading_priority.ToInt() - 1);
						assetHandle.SetLoadingPriority(loading_priority);
					}
				}
			}

			ImGui::SameLine(0, style.ItemInnerSpacing.x);

			// increase priority
			{
				if (loading_priority == AssetLoadingPriority::Critical)
				{
					ImGui::BeginDisabled();
					ImGui::Button("+", square_button_dimentions);
					ImGui::EndDisabled();
				}
				else
				{
					if (ImGui::Button("+", square_button_dimentions))
					{
						loading_priority.FromInt(loading_priority.ToInt() + 1);
						assetHandle.SetLoadingPriority(loading_priority);
					}
				}
			}

			ImGui::SameLine(0, style.ItemInnerSpacing.x);
			ImGui::Text("Loading Priority");
		}

		if (reset_handle)
			assetHandle.SetID(NullAssetID);

		ImGui::EndChild();
		ImGui::PopID();
	}
	template void DataComponent::DrawAssetHandle<Texture2D   >(AssetHandle<Texture2D   >&);
	template void DataComponent::DrawAssetHandle<Material    >(AssetHandle<Material    >&);
	template void DataComponent::DrawAssetHandle<Mesh        >(AssetHandle<Mesh        >&);
	template void DataComponent::DrawAssetHandle<RenderMesh  >(AssetHandle<RenderMesh  >&);
	template void DataComponent::DrawAssetHandle<Model       >(AssetHandle<Model       >&);


	
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