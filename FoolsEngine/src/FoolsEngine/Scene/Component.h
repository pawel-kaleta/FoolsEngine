#pragma once

#include "ECS.h"

#include "FoolsEngine\Core\UUID.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\Tags.h"
#include "FoolsEngine\Math\Transform.h"
#include "FoolsEngine\Renderer\3 - Representation\Camera.h"
#include "FoolsEngine\Renderer\3 - Representation\MaterialInstance.h"
#include "FoolsEngine\Renderer\3 - Representation\Mesh.h"
#include "FoolsEngine\Renderer\3 - Representation\Model.h"
#include "FoolsEngine\Renderer\8 - Render\Renderer2D.h"

#include "FoolsEngine\ImGui\ImGuiLayer.h"

#include "FoolsEngine\Assets\AssetHandle.h"

namespace YAML
{
	class Emitter;
	class Node;
}

namespace fe
{
	namespace SimulationStages
	{
		enum class Stages;
	}
	class BaseEntity;
	

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
		void DrawAssetHandle(AssetHandle<tnAsset>& assetHandle, const std::string& nameTag);
	};

	struct SpatialComponent : DataComponent
	{
		Transform Offset;
		virtual bool IsSpatial() const override final { return true; }

		void SerializeOffset(YAML::Emitter& emitter) const;
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
		      Transform  GetCopy() const { return Transform; }
		const Transform& GetRef()  const{ return Transform; }
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

	struct CRenderMesh final : SpatialComponent
	{
		FE_COMPONENT_SETUP(CRenderMesh, "RenderMesh");

		AssetHandle<Mesh> Mesh;
		AssetHandle<MaterialInstance> MaterialInstance; // Default3D

		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};

	struct CRenderModel final : SpatialComponent
	{
		FE_COMPONENT_SETUP(CRenderModel, "RenderModel");

		AssetHandle<Model> Model;
		// list of material instances?

		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};
}