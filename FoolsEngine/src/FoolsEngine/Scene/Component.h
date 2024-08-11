#pragma once

#include "ECS.h"

#include "FoolsEngine\Core\UUID.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\Tags.h"
#include "FoolsEngine\Math\Transform.h"
#include "FoolsEngine\Renderer\3 - Representation\Camera.h"
#include "FoolsEngine\Renderer\3 - Representation\MaterialInstance.h"
#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"
#include "FoolsEngine\Renderer\8 - Render\Renderer2D.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"
#include "SimulationStages.h"
#include "FoolsEngine\Assets\AssetHandle.h"

#include <yaml-cpp\yaml.h>

namespace fe
{
	class BaseEntity;
	namespace AssetImportModal
	{
		extern void OpenWindow(const std::filesystem::path& filepath, AssetType type, AssetHandleBase* optionalBaseHandle);
	}


#define FE_COMPONENT_SETUP(type, name) \
	virtual std::string GetName() const override final { return name; } \
	static std::string GetNameStatic() { return name; }

	struct Component {};

	struct FlagComponent : Component {};

	struct DataComponent : Component
	{
		virtual std::string GetName() const = 0;
		static std::string GetNameStatic() { return "Uknown DataComponent"; }
		virtual bool IsSpatial() const { return false; }

		virtual void SerializeBase(YAML::Emitter& emitter) { Serialize(emitter); }
		virtual void DeserializeBase(YAML::Node& data) { Deserialize(data); }

		virtual void DrawInspectorWidget(BaseEntity entity);
		virtual void Serialize(YAML::Emitter& emitter);
		virtual void Deserialize(YAML::Node& data);

	protected:
		template<typename tnAsset>
		void DrawAssetHandle(AssetHandle<tnAsset>& assetHandle, const std::string& nameTag)
		{
			std::string name;
			if (!assetHandle.IsValid())
			{
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.25f,0.25f,0.25f,1.0f });
				name = "-";
			}
			else
			{
				name = std::to_string(assetHandle.GetID()) + ": " + assetHandle.Observe().GetName();
			}
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FrameBorderSize, 2.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
			bool selected = ImGui::Button(name.c_str(), { ImGui::GetContentRegionAvail().x / 2, ImGui::GetTextLineHeightWithSpacing() });

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetPath"))
				{
					IM_ASSERT(payload->DataSize == sizeof(std::filesystem::path));
					std::filesystem::path filepath = *(const std::filesystem::path*)payload->Data;
					if (!filepath.empty())
					{
						std::filesystem::path extension = filepath.extension();
						
						if (extension == tnAsset::GetProxyExtension())
						{
							AssetID newAssetID = AssetManager::GetOrCreateAsset<tnAsset>(filepath);
							assetHandle = AssetHandle<tnAsset>(newAssetID);
							if (tnAsset::GetTypeStatic() == AssetType::Texture2DAsset)
							{
								auto textureUser = assetHandle.Use();
								TextureLoader::LoadTexture(textureUser);
								// ^ needed to create specification, should happen inside GetOrCreateAsset with some array of funk ptrs to AssetType specific init funks
							}
						}
						else if (tnAsset::IsKnownSourceExtension(extension))
						{
							#ifdef FE_EDITOR
								AssetImportModal::OpenWindow(filepath, tnAsset::GetTypeStatic(), &assetHandle);
							#endif // FE_EDITOR
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::Text(nameTag.c_str());
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			if (!assetHandle.IsValid())
				ImGui::PopStyleColor();
		}
	};

	struct SpatialComponent : DataComponent
	{
		Transform Offset;
		virtual bool IsSpatial() const override final { return true; }

		void SerializeOffset(YAML::Emitter& emitter);
		void DeserializeOffset(YAML::Node& data);

		virtual void SerializeBase(YAML::Emitter& emitter) override final { SerializeOffset(emitter); Serialize(emitter); }
		virtual void DeserializeBase(YAML::Node& data)     override final { DeserializeOffset(data); Deserialize(data); }
	};

	struct ProtectedComponent : DataComponent
	{
		virtual bool IsSpatial() const override final { return false; }
	};

	struct CUUID final : ProtectedComponent
	{
		UUID UUID;

		FE_COMPONENT_SETUP(CUUID, "UUID");
	};

	struct CEntityName final : ProtectedComponent
	{
		CEntityName(const std::string& name)
			: EntityName(name) {}

		std::string EntityName = std::string();

		operator       std::string& () { return EntityName; }
		operator const std::string& () { return EntityName; }

		FE_COMPONENT_SETUP(CEntityName, "EntityName");
	};

	struct HierarchyNode : ProtectedComponent
	{
		EntityID Parent				= RootID;
		uint32_t HierarchyLvl		= 0;

		EntityID PreviousSibling	= NullEntityID;
		EntityID NextSibling		= NullEntityID;

		uint32_t ChildrenCount		= 0;
		EntityID FirstChild			= NullEntityID;
	};

	struct CEntityNode final : HierarchyNode
	{
		FE_COMPONENT_SETUP(CEntityNode, "EntityNode");
	};

	struct CActorNode final : HierarchyNode
	{
		FE_COMPONENT_SETUP(CActorNode, "ActorNode");
	};

	struct CHeadEntity final : ProtectedComponent
	{
		EntityID HeadEntity = NullEntityID;

		FE_COMPONENT_SETUP(CHeadEntity, "HeadEntity");
	};

	struct CTags final : ProtectedComponent
	{
		operator const Tags& () const { return Global; }

		FE_COMPONENT_SETUP(CTags, "Tags");

		Tags Global;
		Tags Local;
	};

	struct TransformComponent : ProtectedComponent
	{
	public:
		      Transform  GetCopy() { return Transform; }
		const Transform& GetRef()  { return Transform; }
	private:
		friend class TransformHandle;
		friend class HierarchyDirector;
		friend class SceneSerializerYAML;

		Transform Transform;
	};

	struct CTransformLocal final : public TransformComponent
	{
		FE_COMPONENT_SETUP(CTransformLocal, "TransformLocal");
	};

	struct CTransformGlobal final : public TransformComponent
	{
		FE_COMPONENT_SETUP(CTransformGlobal, "TransformGlobal");
	};

	struct CCamera final : SpatialComponent
	{
		Camera Camera;
		bool IsPrimary = false;

		FE_COMPONENT_SETUP(CCamera, "Camera");
		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};

	struct CTile final : SpatialComponent
	{
		Renderer2D::Quad Tile;

		FE_COMPONENT_SETUP(CTile, "Tile");
		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};

	struct CSprite final : SpatialComponent
	{
		Renderer2D::Quad Sprite;

		FE_COMPONENT_SETUP(CSprite, "Sprite");
		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};

	struct CDestroyFlag final : FlagComponent {};

	template <typename tnComponent>
	struct CDirtyFlag final : FlagComponent {};

	template <SimulationStages::Stages stage>
	struct CUpdateEnrollFlag final : FlagComponent {};

	struct CMesh final : SpatialComponent
	{
		FE_COMPONENT_SETUP(CMesh, "Mesh");

		AssetHandle<Mesh> Mesh;

		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};

	struct CMaterialInstance final : DataComponent
	{
		FE_COMPONENT_SETUP(CMaterialInstance, "MaterialInstance");

		AssetHandle<MaterialInstance> MaterialInstance;

		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};
}