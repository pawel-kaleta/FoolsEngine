#include "FE_pch.h"

#include "Component.h"

#include "BaseEntity.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "World.h"
#include "Scene.h"
#include "FoolsEngine\Platform\FileDialogs.h"
#include "FoolsEngine\Assets\MeshImporter.h"

#include "FoolsEngine\Assets\Serializers\SceneSerializer.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

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
		auto flat_color_texture = (AssetID)BaseAssets::Textures2D::FlatWhite;

		std::string combo_preview_value = Tile.Texture.GetID() == flat_color_texture ? "None" : std::to_string(Tile.Texture.GetID()) + ": " + Tile.Texture.Observe().GetName();

		if (ImGui::BeginCombo("Texture", combo_preview_value.c_str()))
		{
			auto item_current = Tile.Texture;
			bool is_selected = (Tile.Texture.GetID() == flat_color_texture);

			if (ImGui::Selectable("None", is_selected))
				item_current = AssetHandle<Texture2D>(flat_color_texture);

			auto textures = AssetManager::GetAll<Texture2D>();
			for (auto id : textures)
			{
				auto texture = AssetHandle<Texture2D>(id);
				auto textureObserver = texture.Observe();

				if (textureObserver.GetSpecification()->Specification.Components != TextureData::Components::RGB)
					continue;

				if (id == flat_color_texture)
					continue;

				is_selected = (item_current.GetID() == id);
				std::string name = std::to_string(id) + ": " + textureObserver.GetName();
				if (ImGui::Selectable(name.c_str(), is_selected))
					item_current = texture;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			if (ImGui::Selectable("Add..."))
			{
				const std::filesystem::path newTextureProxyFilepath = FileDialogs::OpenFile("(*.fetex2d)\0*.fetex2d\0");
				if (!newTextureProxyFilepath.empty())
				{
					AssetID newTextureID = AssetManager::GetOrCreateAsset<Texture2D>(newTextureProxyFilepath);
					auto texture = AssetHandle<Texture2D>(newTextureID);
					auto textureUser = texture.Use();
					TextureLoader::LoadTexture(textureUser);
					// ^ needed to create specification, should happen inside GetOrCreateAsset with some array of funk ptrs to AssetType specific init funks

					if (textureUser.GetSpecification()->Specification.Components == TextureData::Components::RGB)
						item_current = texture;
				}
			}
			Tile.Texture = item_current;

			ImGui::EndCombo();
		}

		if (Tile.Texture.GetID() == flat_color_texture)
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

		auto flat_color_texture = (AssetID)BaseAssets::Textures2D::FlatWhite;
		std::string combo_preview_value = Sprite.Texture.GetID() == flat_color_texture ? "None" : std::to_string(Sprite.Texture.GetID()) + ": " + Sprite.Texture.Observe().GetName();

		if (ImGui::BeginCombo("Texture", combo_preview_value.c_str()))
		{
			auto item_current = Sprite.Texture;
			bool is_selected = (Sprite.Texture.GetID() == flat_color_texture);

			if (ImGui::Selectable("None", is_selected))
				item_current = AssetHandle<Texture2D>(flat_color_texture);

			auto textures = AssetManager::GetAll<Texture2D>();
			for (auto id : textures)
			{
				auto texture = AssetHandle<Texture2D>(id);
				if (id == flat_color_texture)
					continue;

				is_selected = (item_current.GetID() == id);
				std::string name = std::to_string(id) + ": " + texture.Observe().GetName();
				if (ImGui::Selectable(name.c_str(), is_selected))
					item_current = texture;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			if (ImGui::Selectable("Add..."))
			{
				const std::filesystem::path newTextureProxyFilepath = FileDialogs::OpenFile("(*.fetex2d)\0*.fetex2d\0");
				if (!newTextureProxyFilepath.empty())
				{
					AssetID newTextureID = AssetManager::GetOrCreateAsset<Texture2D>(newTextureProxyFilepath);
					item_current = AssetHandle<Texture2D>(newTextureID);
					auto textureUser = item_current.Use();
					TextureLoader::LoadTexture(textureUser);
					// ^ needed to create specification, should happen inside GetOrCreateAsset with some array of funk ptrs to AssetType specific init funks
				}
			}
			Sprite.Texture = item_current;
			ImGui::EndCombo();
		}

		if (Sprite.Texture.GetID() == flat_color_texture)
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

	void CMesh::DrawInspectorWidget(BaseEntity entity)
	{
		const char* combo_preview_value = !Mesh.IsValid() ? "None" : Mesh.Observe().GetName().c_str();
		if (ImGui::BeginCombo("Mesh ", combo_preview_value))
		{
			bool is_selected = !(Mesh.IsValid());

			if (ImGui::Selectable("None", is_selected))
				Mesh = AssetHandle<fe::Mesh>();

			auto meshes = AssetManager::GetAll<fe::Mesh>();

			for (auto id : meshes)
			{
				auto nextMesh = AssetHandle<fe::Mesh>(id);
				is_selected = Mesh.IsValid() ? (Mesh.GetID() == id) : false;

				if (ImGui::Selectable(nextMesh.Observe().GetName().c_str(), is_selected))
				{
					Mesh = nextMesh;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			if (ImGui::Selectable("Load..."))
			{
				const std::filesystem::path modelFilepath = FileDialogs::OpenFile("(*.*)\0*.*\0");
				if (!modelFilepath.empty())
				{
					FE_CORE_ASSERT(false, "");
					// markmark
					//auto newMeshes = ModelImporter::Import(modelFilepath);
					//for (auto& mesh : newMeshes)
					//{
					//	if (mesh->GetVertexCount())
					//		MeshLibrary::Add(mesh);
					//}
				}
			}

			ImGui::EndCombo();
		}
	}

	void CMesh::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Mesh" << YAML::Value << Mesh; 
	}

	void CMesh::Deserialize(YAML::Node& data)
	{ 
		Mesh = data[Mesh].as<AssetHandle<fe::Mesh>>();
	}

	void CMaterialInstance::DrawInspectorWidget(BaseEntity entity)
	{
		// TO DO: component's widget should not be responsible for creating underlying object
		std::string previewName = MaterialInstance.Observe().GetName();
		const char* materialInstance_combo_preview = previewName.c_str();

		if (ImGui::BeginCombo("Material Instance", materialInstance_combo_preview))
		{
			bool is_selected;

			auto materialInstances = AssetManager::GetAll<fe::MaterialInstance>();
			for (auto id : materialInstances)
			{
				auto nextMatInst = AssetHandle<fe::MaterialInstance>(id);
				
				is_selected = (MaterialInstance.GetID() == id);

				if (ImGui::Selectable(nextMatInst.Observe().GetName().c_str(), is_selected))
				{
					MaterialInstance = nextMatInst;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			if (ImGui::Selectable("Add..."))
			{
				// markmark
				// create file / read file
				std::filesystem::path filePath;
				MaterialInstance = AssetHandle<fe::MaterialInstance>(AssetManager::GetOrCreateAsset<fe::MaterialInstance>(filePath));
				auto miUser = MaterialInstance.Use();
				MaterialInstance::MakeMaterialInstance(miUser);
				auto material = AssetHandle<Material>((AssetID)BaseAssets::Materials::Default3D);
				miUser.Init(material);
			}

			ImGui::EndCombo();
		}

		static char buffer[256];
		{
			auto miObserver = MaterialInstance.Observe();
			auto& name = miObserver.GetName();
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, name.c_str(), sizeof(buffer));
		}
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
		{
			FE_CORE_ASSERT(false, "What? Should name change be happening here at all? Is this an artifact of prototyping?");
			// markmark
			//MaterialInstance.Use().GetName()SetName(buffer);
		}

		auto miUser = MaterialInstance.Use();
		auto material_current = miUser.GetMaterial();

		if (ImGui::BeginCombo("Material", material_current.Observe().GetName().c_str()))
		{
			bool is_selected;

			auto materials = AssetManager::GetAll<Material>();

			for (auto id : materials)
			{
				AssetHandle<Material> materialHandle(id);
				is_selected = (material_current.GetID() == id);

				if (ImGui::Selectable(materialHandle.Observe().GetName().c_str(), is_selected))
				{
					miUser.Init(materialHandle);
					material_current = materialHandle;
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

			const char* texture_combo_preview = !texture_current.IsValid() ? "None" : texture_current.Observe().GetName().c_str();
			if (ImGui::BeginCombo(textureSlot.GetName().c_str(), texture_combo_preview))
			{
				bool is_selected = !(texture_current.IsValid());

				if (ImGui::Selectable("None", is_selected))
					miUser.SetTexture(textureSlot, AssetHandle<Texture2D>());

				auto textures = AssetManager::GetAll<Texture2D>();
				for (auto id : textures)
				{
					auto textureHandle = AssetHandle<Texture2D>(id);
					is_selected = (texture_current.GetID() == textureHandle.GetID());

					if (ImGui::Selectable(textureHandle.Observe().GetName().c_str(), is_selected))
					{
						newSelection = true;
						texture_current = textureHandle;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				if (ImGui::Selectable("Add..."))
				{
					const std::filesystem::path newTextureFilepath = FileDialogs::OpenFile("(*.*)\0*.*\0");
					if (!newTextureFilepath.empty())
					{
						//to do: detecting reloading same texture

						FE_CORE_ASSERT(false, "");
						// markmark
						
						//const std::string textureName = FileNameFromFilepath(newTextureFilepath.string());
						//if (!TextureLibrary::Exist(textureName))
						//{

							//Ref<Texture> texture = Texture2D::Create(newTextureFilepath.string(), TextureData::Usage::Map_Albedo);
							//TextureLibrary::Add(texture);
						//}
						
						//texture_current = TextureLoader::LoadTexture(newTextureFilepath);
						//newSelection = true;
					}
				}

				ImGui::EndCombo();
			}

			if (newSelection)
			{
				miUser.SetTexture(textureSlot, texture_current);
			}
		}
	}

	void CMaterialInstance::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "MaterialInstance" << YAML::Value << MaterialInstance;
	}

	void CMaterialInstance::Deserialize(YAML::Node& data)
	{
		MaterialInstance = data[MaterialInstance].as<AssetHandle<fe::MaterialInstance>>();
	}

	void SpatialComponent::SerializeOffset(YAML::Emitter& emitter)
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
}