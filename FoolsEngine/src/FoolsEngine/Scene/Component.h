#pragma once

#include "ECS.h"
#include "SimulationStage.h"

#include "FoolsEngine\Core\UUID.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\Tags.h"
#include "FoolsEngine\Math\Transform.h"

#include "FoolsEngine\ImGui\ImGuiLayer.h"

#include "FoolsEngine\Assets\AssetHandle.h"

namespace YAML
{
	class Emitter;
	class Node;
}

namespace fe
{
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

		virtual void SerializeBase(YAML::Emitter& emitter) { Serialize(emitter); } // SpatialComponent overrides
		virtual void DeserializeBase(YAML::Node& data)     { Deserialize(data); }

		virtual void Serialize(YAML::Emitter& emitter);
		virtual void Deserialize(YAML::Node& data);

		virtual void DrawInspectorWidget(BaseEntity entity);

	protected:
		template<typename tnAsset>
		void DrawAssetHandle(const AssetHandle<tnAsset>& assetHandle, const std::string& nameTag);
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



	struct CDestroyFlag final : FlagComponent {};

	template <typename tnComponent>
	struct CDirtyFlag final : FlagComponent {};

	template <SimulationStage::ValueType stage>
	struct CUpdateEnrollFlag final : FlagComponent {};

}