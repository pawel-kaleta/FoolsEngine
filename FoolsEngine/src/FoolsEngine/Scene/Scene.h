#pragma once
#include "ECS.h"

#include "FoolsEngine\Scene\GameplayWorld\GameplayWorld.h"
#include "FoolsEngine\Core\UUID.h"

namespace fe
{
	namespace SimulationStages
	{
		enum class Stage;
	}

	class Entity;

	class Scene : public Asset
	{
	public:

		// Asset
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static AssetType GetTypeStatic() { return AssetType::SceneAsset; }
		static bool IsKnownSourceExtension(const std::filesystem::path& extension) { return false; }
		//static std::string GetSourceExtensionAlias() { return ""; } // base class Asset asserts here and its ok
		static std::string GetProxyExtension() { return ".fescene"; }
		static std::string GetProxyExtensionAlias() { return "Scene"; }

		virtual void UnloadFromGPU() override { FE_CORE_ASSERT(false, "Why is this called?"); }
		virtual void UnloadFromCPU() override { FE_CORE_ASSERT(false, "Why is this called?"); }

		Scene();
		~Scene() = default;
		void Initialize();

		void SimulationUpdate();
		void PostFrameUpdate();

		GameplayWorld* GetGameplayWorld() const { return m_GameplayWorld.get(); }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		const std::filesystem::path& GetFilepath() const { return m_Filepath; }
		void SetFilepath(const std::filesystem::path& path) { m_Filepath = path; }
		      
		UUID GetUUID() const { return m_UUID; }
	private:
		friend class SceneSerializerYAML;

		std::filesystem::path m_Filepath;
		std::string           m_Name = "Untitled Scene";
		UUID                  m_UUID;
		
		Scope<GameplayWorld>  m_GameplayWorld;
		
		template <SimulationStages::Stages stage>
		void Update();
	};
}