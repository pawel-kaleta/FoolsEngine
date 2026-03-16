#include "FE_pch.h"
#include "Component.h"

#include "BaseEntity.h"
#include "GameplayWorld/Entity.h"

#include "FoolsEngine/Foundation/Memory/Scratchpad.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Assets/Serialization/YAML.h"
#include "FoolsEngine/Assets/Loaders/LoadersRegistry.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/4 - Representation/Mesh.h"
#include "FoolsEngine/Renderer/4 - Representation/Model.h"
#include "FoolsEngine/Renderer/4 - Representation/Texture.h"
#include "FoolsEngine/Renderer/4 - Representation/Material.h"
#include "FoolsEngine/Renderer/4 - Representation/RenderMesh.h"

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

	static bool EditMaterial(const AssetUser<Material>& materialUser)
	{
		Scratchpad sp;

		bool modified = false;

		auto& shading_model_current = materialUser.GetCoreComponent().ShadingModelID;
		if (ImGui::BeginCombo("Shading Model", AssetObserver<ShadingModel>(shading_model_current).GetFilepath().filename().string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str()))
		{
			bool is_selected;

			auto shading_models = AssetManager::Get().m_Registry.view<ACShadingModelCore>();

			Scratchpad sp2;

			for (auto id : shading_models)
			{
				auto shading_model_observer = AssetObserver<ShadingModel>(id);
				is_selected = (shading_model_current == id);

				if (ImGui::Selectable(shading_model_observer.GetFilepath().filename().string<PMR_STRING_TEMPLATE_PARAMS>(&sp2).c_str(), is_selected))
				{
					materialUser.MakeMaterial(shading_model_observer);
					shading_model_current = id;
					modified = true;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		AssetObserver<ShadingModel> shading_model_observer(shading_model_current);

		auto& sm_core_component = shading_model_observer.GetCore();
		auto& material_core_component = materialUser.GetCoreComponent();

		const auto& lib = Description::Library::Get();
		const auto& program_spec = lib.ProgramSpecs[sm_core_component.ProgramSpecificationID];
		const auto& uniforms_layout = lib.BufferLayouts[program_spec.MainUniformsLayoutID];

		for (auto& uniform : uniforms_layout.Elements)
		{
			if (ImGuiLayer::RenderUniform(uniform, materialUser.GetUniformValuePtr(material_core_component, uniform)))
				modified = true;
		}

		for (size_t i=0; i<material_core_component.TextureIDs.size(); i++)
		{
			auto& textureID = material_core_component.TextureIDs[i];
			
			ImGui::PushID((const void*)&textureID);
			
			ImGuiStyle& style = ImGui::GetStyle();
			const float square_button_size = ImGui::GetFrameHeight();
			const ImVec2 square_button_dimentions = { square_button_size, square_button_size };
			const float button_width__handle = (ImGui::GetContentRegionAvail().x / 2) - (style.ItemInnerSpacing.x + square_button_size);
			bool handle_valid = textureID != NullAssetID;
			bool reset_handle = false;

			reset_handle = ImGui::Button("x", square_button_dimentions); ImGui::SameLine(0, style.ItemInnerSpacing.x);

			std::pmr::string asset_name(&sp);
			asset_name = handle_valid ? AssetObserver<Texture2D>(textureID).GetFilepath().stem().string<PMR_STRING_TEMPLATE_PARAMS>(&sp) : "<empty>";

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

							if (Texture2D::GetMetaFileExtension() == extension)
							{
								AssetID assetID = AssetManager::GetAssetFromFilepath(filepath.lexically_relative(Project::Get()->m_AssetsPath));
								if (assetID != NullAssetID)
								{
									textureID = assetID;
									modified = true;
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

				if (reset_handle)
				{
					textureID = NullAssetID;
					modified = true;
				}
			}

			const auto& library = Description::Library::Get();
			const auto& program_spec = library.ProgramSpecs[sm_core_component.ProgramSpecificationID];
			const auto& texture_sampler = library.TextureSamplers[program_spec.TextureSamplerIDs[i]];
			ImGui::Text(texture_sampler.Name.c_str());

			ImGui::PopID();
		}

		return modified;		
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
								AssetID assetID = AssetManager::GetAssetFromFilepath(filepath.lexically_relative(Project::Get()->m_AssetsPath));
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

		if (handle_valid)
		{
			if constexpr (tnAsset::GetTypeStatic() == AssetType::Texture2D)
			{
				//
			}
			if constexpr (tnAsset::GetTypeStatic() == AssetType::Material)
			{
				ImGui::SeparatorText("Material parameters");
				if (EditMaterial(assetHandle.Use()))
				{
					YAML::Emitter emitter;
					Material::SaveMetadata(emitter, assetHandle.GetID());
					std::ofstream fout(Project::Get()->m_AssetsPath / assetHandle.Observe().GetFilepath());
					fout << emitter.c_str();
				}
			}
		}

		if (reset_handle)
			assetHandle.SetID(NullAssetID);

		ImGui::EndChild();
		ImGui::PopID();
	}

#define _DRAW_ASSET_HANDLE_DEF(x) template void DataComponent::DrawAssetHandle<x>(AssetHandle<x>&);
	FE_FOR_EACH(_DRAW_ASSET_HANDLE_DEF, FE_ASSET_TYPES_LIST);
}