#include "FE_pch.h"
#include "Component.h"

#include "BaseEntity.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "World.h"
#include "Scene.h"

#include "FoolsEngine\Assets\Serializers\YAML.h"
#include "FoolsEngine\Assets\Serializers\SceneSerializer.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"
#include "FoolsEngine\Assets\Loaders\GeometryLoader.h"

#include "../../FoolsTools/src/AssetImport/FileHandler.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include <type_traits>

#include <glm\gtc\type_ptr.hpp>

namespace fe
{
	namespace AssetImportModal
	{
		extern void OpenWindow(const std::filesystem::path& filepath, AssetType type, AssetHandleBase* optionalBaseHandle);
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
			name = std::to_string(assetHandle.GetID()) + ": " + assetHandle.Observe().TryGetName()->Name;
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

					uint32_t file_importer_idx = FileHandler::GetSourceAliasAndLoaderIndex(extension, std::pmr::string());
					bool     is_asset_proxy    = FileHandler::GetAliasAndAssetTypeOfProxy(extension, std::pmr::string()) != AssetType::None;

					if (is_asset_proxy || file_importer_idx != -1)
					{
						AssetID assetID = AssetManager::GetAssetFromFilepath<tnAsset>(filepath);
						if (assetID != NullAssetID)
						{
							assetHandle = AssetHandle<tnAsset>(assetID);
						}
						else
						{
							FE_CORE_ASSERT(!is_asset_proxy, "Dont import asset proxies!");

							uint32_t file_importer_idx = FileHandler::GetSourceAliasAndLoaderIndex(extension, std::pmr::string());
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
	template void DataComponent::DrawAssetHandle<Texture2D>(AssetHandle<Texture2D>&, const std::string&);
	template void DataComponent::DrawAssetHandle<Shader>(AssetHandle<Shader>&, const std::string&);
	template void DataComponent::DrawAssetHandle<Mesh>(AssetHandle<Mesh>&, const std::string&);

	void CCamera::DrawInspectorWidget(BaseEntity entity)
	{
		auto* world = entity.GetWorld();

		if (world->IsGameplayWorld())
		{
			if (entity.ID() == ((GameplayWorld*)world)->GetEntityWithPrimaryCamera().ID())
			{
				IsPrimary = true;
			}

			if (ImGui::Checkbox("Primary", &IsPrimary))
				if (IsPrimary)
					((GameplayWorld*)world)->SetPrimaryCameraEntity(Entity(entity));
		}

		constexpr char* projectionTypeStrings[] = { "Orthographic", "Perspective" };
		const char* currentProjectionTypeString = projectionTypeStrings[(int)Camera.GetProjectionType()];

		if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
				{
					currentProjectionTypeString = projectionTypeStrings[i];
					Camera.SetProjectionType((Camera::ProjectionType)i);
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (Camera.GetProjectionType() == Camera::ProjectionType::Perspective)
		{
			float verticalFov = glm::degrees(Camera.GetPerspectiveFOV());
			if (ImGui::DragFloat("Field of View", &verticalFov))
				Camera.SetPerspectiveFOV(glm::radians(verticalFov));

			float orthoNear = Camera.GetPerspectiveNearClip();
			if (ImGui::DragFloat("Near Clip", &orthoNear))
				Camera.SetPerspectiveNearClip(orthoNear);

			float orthoFar = Camera.GetPerspectiveFarClip();
			if (ImGui::DragFloat("Far Clip", &orthoFar))
				Camera.SetPerspectiveFarClip(orthoFar);
		}
		else
		{
			float zoom = Camera.GetOrthographicZoom();
			if (ImGui::DragFloat("Zoom", &zoom))
				Camera.SetOrthographicZoom(zoom);

			float orthoNear = Camera.GetOrthographicNearClip();
			if (ImGui::DragFloat("Near Clip", &orthoNear))
				Camera.SetOrthographicNearClip(orthoNear);

			float orthoFar = Camera.GetOrthographicFarClip();
			if (ImGui::DragFloat("Far Clip", &orthoFar))
				Camera.SetOrthographicFarClip(orthoFar);
		}
	}

	void CCamera::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "IsPrimary"        << YAML::Value << IsPrimary;

		emitter << YAML::Key << "ProjectionType"   << YAML::Value << Camera.GetProjectionType();

		emitter << YAML::Key << "PerspectiveNear"  << YAML::Value << Camera.GetPerspectiveNearClip();
		emitter << YAML::Key << "PerspectiveFar"   << YAML::Value << Camera.GetPerspectiveFarClip();
		emitter << YAML::Key << "PerspectiveFOV"   << YAML::Value << Camera.GetPerspectiveFOV();

		emitter << YAML::Key << "OrthographicNear" << YAML::Value << Camera.GetOrthographicNearClip();
		emitter << YAML::Key << "OrthographicFar"  << YAML::Value << Camera.GetOrthographicFarClip();
		emitter << YAML::Key << "OrthographicZoom" << YAML::Value << Camera.GetOrthographicZoom();
	}

	void CCamera::Deserialize(YAML::Node& data)
	{
		IsPrimary = data["IsPrimary"].as<bool>();

		Camera.SetProjectionType((Camera::ProjectionType)data["ProjectionType"].as<int>());
			   
		Camera.SetPerspectiveNearClip( data["PerspectiveNear" ].as<float>());
		Camera.SetPerspectiveFarClip(  data["PerspectiveFar"  ].as<float>());
		Camera.SetPerspectiveFOV(      data["PerspectiveFOV"  ].as<float>());
								   
		Camera.SetOrthographicNearClip(data["OrthographicNear"].as<float>());
		Camera.SetOrthographicFarClip( data["OrthographicFar" ].as<float>());
		Camera.SetOrthographicZoom(    data["OrthographicZoom"].as<float>());
	}

	void CTile::DrawInspectorWidget(BaseEntity entity)
	{
		std::string nameTag = "Texture";
		DrawAssetHandle<Texture2D>(Tile.Texture, nameTag);
		
		if (Tile.Texture == Renderer::BaseAssets.Textures.FlatWhite)
		{
			ImGui::ColorEdit3("Color", glm::value_ptr(Tile.Color));
		}
		else
		{
			ImGui::DragFloat("Tiling Factor", &Tile.TextureTilingFactor, 0.1f);

			ImGui::ColorEdit3("Tint Color", glm::value_ptr(Tile.Color));
		}
	}

	void CTile::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Color"   << YAML::Value << Tile.Color;
		emitter << YAML::Key << "Texture" << YAML::Value << Tile.Texture;
		emitter << YAML::Key << "Tiling"  << YAML::Value << Tile.TextureTilingFactor;
	}

	void CTile::Deserialize(YAML::Node& data)
	{
		Tile.Color = data["Color"].as<glm::vec4>();
		Tile.Texture = data["Texture"].as<AssetHandle<Texture2D>>();
		Tile.TextureTilingFactor = data["Tiling"].as<float>();
	}

	void CSprite::DrawInspectorWidget(BaseEntity entity)
	{
		std::string nameTag = "Texture";
		DrawAssetHandle<Texture2D>(Sprite.Texture, nameTag);

		if (Sprite.Texture == Renderer::BaseAssets.Textures.FlatWhite)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(Sprite.Color));
		}
		else
		{
			ImGui::DragFloat("Tiling Factor", &Sprite.TextureTilingFactor, 0.1f);

			ImGui::ColorEdit4("Tint Color", glm::value_ptr(Sprite.Color));
		}
	}

	void CSprite::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Color"   << YAML::Value << Sprite.Color;
		emitter << YAML::Key << "Texture" << YAML::Value << Sprite.Texture;
		emitter << YAML::Key << "Tiling"  << YAML::Value << Sprite.TextureTilingFactor;
	}

	void CSprite::Deserialize(YAML::Node& data)
	{
		Sprite.Color = data["Color"].as<glm::vec4>();
		Sprite.Texture = data["Texture"].as<AssetHandle<Texture2D>>();
		Sprite.TextureTilingFactor = data["Tiling"].as<float>();
	}

	void CRenderMesh::DrawInspectorWidget(BaseEntity entity)
	{
		std::string mesh_name_tag = "Mesh";
		DrawAssetHandle<fe::Mesh>(Mesh, mesh_name_tag);
		std::string mi_name_tag = "Material Instance";
		DrawAssetHandle<fe::MaterialInstance>(MaterialInstance, mi_name_tag);
	}

	void CRenderMesh::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Mesh"             << YAML::Value << Mesh; 
		emitter << YAML::Key << "MaterialInstance" << YAML::Value << MaterialInstance;
	}

	void CRenderMesh::Deserialize(YAML::Node& data)
	{ 
		Mesh = data["Mesh"].as<AssetHandle<fe::Mesh>>();
		MaterialInstance = data["MaterialInstance"].as<AssetHandle<fe::MaterialInstance>>();
	}

	void EditMaterialInstance()
	{
		AssetHandle<MaterialInstance> MaterialInstance;

		if (!MaterialInstance.IsValid())
		{
			return;
		}

		auto miUser = MaterialInstance.Use();
		auto material_current = miUser.GetMaterial();

		if (ImGui::BeginCombo("Material", material_current.Observe().TryGetName()->Name.c_str()))
		{
			bool is_selected;

			auto materials = AssetManager::GetAll<Material>();

			for (auto id : materials)
			{
				AssetHandle<Material> material_handle(id);
				auto material_observer = material_handle.Observe();
				is_selected = (material_current.GetID() == id);

				if (ImGui::Selectable(material_observer.TryGetName()->Name.c_str(), is_selected))
				{
					miUser.Init(material_observer);
					material_current = material_handle;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		auto materialObserver = miUser.GetMaterial().Observe();

		auto& uniforms = materialObserver.GetUniforms();

		for (auto& uniform : uniforms)
		{
			ImGuiLayer::RenderUniform(uniform, miUser.GetUniformValuePtr(uniform));
		}

		auto& textureSlots = materialObserver.GetTextureSlots();

		for (auto& textureSlot : textureSlots)
		{
			auto texture_current = miUser.GetTexture(textureSlot);
			bool newSelection = false;

			const char* texture_combo_preview = !texture_current.IsValid() ? "None" : texture_current.Observe().TryGetName()->Name.c_str();
			if (ImGui::BeginCombo(textureSlot.GetName().c_str(), texture_combo_preview))
			{
				bool is_selected = !(texture_current.IsValid());

				if (ImGui::Selectable("None", is_selected))
					miUser.SetTexture(textureSlot, NullAssetID);

				auto textures = AssetManager::GetAll<Texture2D>();
				for (auto id : textures)
				{
					auto textureHandle = AssetHandle<Texture2D>(id);
					is_selected = (texture_current.GetID() == textureHandle.GetID());

					if (ImGui::Selectable(textureHandle.Observe().TryGetName()->Name.c_str(), is_selected))
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
				miUser.SetTexture(textureSlot, texture_current.GetID());
			}
		}
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
		auto& node = data["Offset"];
		Offset.Shift = node["Shift"].as<glm::vec3>();
		Offset.Rotation = node["Rotation"].as<glm::vec3>();
		Offset.Scale = node["Scale"].as<glm::vec3>();
	}
	
	void CRenderModel::DrawInspectorWidget(BaseEntity entity) {}
	void CRenderModel::Serialize(YAML::Emitter& emitter) {}
	void CRenderModel::Deserialize(YAML::Node& data) {}
}