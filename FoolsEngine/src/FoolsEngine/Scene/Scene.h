#pragma once
#include "ECS.h"

#include "SimulationStage.h"

#include "FoolsEngine/Scene/GameplayWorld/GameplayWorld.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetInterface.h"

namespace YAML { class Emitter; }

namespace fe
{
	class Entity;

	struct ACSceneCore : public AssetComponent
	{
		Scope<GameplayWorld> GameplayWorld;

		void Init() { }
	};

	// observer can be used for drawing editor panels and other stuff during editing/playing scene
	// user needed for initialization, serialization, deserialization and release
	class SceneObserver : public AssetInterface
	{
	public:
		const ACSceneCore& GetCoreComponent() const { return Get<ACSceneCore>(); }

	protected:
		SceneObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class SceneUser : public SceneObserver
	{
	public:
		ACSceneCore& GetCoreComponent() const { return Get<ACSceneCore>(); }
		
		void Initialize() const;
		void InitializeNew() const;
		void Release() const;

		void SimulationUpdate() const;
		void PostFrameUpdate() const;

	private:
		template <SimulationStage::ValueType stage>
		void Update() const;

	protected:
		SceneUser(ECS_AssetHandle ECS_handle) : SceneObserver(ECS_handle) {}
	};

	class Scene : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Scene; }
		static constexpr const char* GetMetaFileExtension() { return ".fescene"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACSceneCore>(assetID).Init(); }
		static void SaveMetadata(YAML::Emitter& emitter, AssetID assetID) { };
		static bool LoadMetadata(AssetID assetID) { return true; }; //TO DO: call SceneSerializer?

		using Observer = SceneObserver;
		using User = SceneUser;
		using Core = ACSceneCore;
	};
}