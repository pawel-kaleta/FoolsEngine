#pragma once

#include "System.h"
#include "FoolsEngine\Scene\SimulationStage.h"


namespace fe
{
	class UUID;

	class SystemsDirector
	{
	public:
		SystemsDirector() = default;
		SystemsDirector(const SystemsDirector& other) = delete;

		template<SimulationStage::ValueType stage>
		void EnrollForUpdate(System* system, void (System::* onUpdateFuncPtr)(), uint32_t priority);

		template<SimulationStage::ValueType stage>
		void RemoveUpdateEnroll(System* system);

		void UpdateSystems(SimulationStage stage);

		template <typename tnSystem>
		tnSystem* CreateSystem()
		{
			FE_PROFILER_FUNC();
			FE_LOG_CORE_DEBUG("System creation");
			static_assert(std::is_base_of_v<System, tnSystem>, "This is not a system!");

			tnSystem* system = new tnSystem();
			m_Systems.emplace_back((System*)system);
			
			system->m_SystemsDirector = this;
			system->OnInitialize();

			return system;
		}

		System* GetSystemFromName(const std::string& name);

		System* GetSystemFromUUID(UUID uuid) const;
	private:
		friend class SystemsInspector;
		friend class SystemsRegistry;
		friend class SceneSerializerYAML;

		using Systems = std::vector<std::unique_ptr<System>>;

		struct SystemUpdateEnroll
		{
			System* System;
			void (System::* OnUpdateFuncPtr)();
			uint32_t Priority;
		};
		using SystemUpdateEnrolls = std::array<std::vector<SystemUpdateEnroll>, SimulationStage::Count>;

		Systems             m_Systems;
		SystemUpdateEnrolls	m_SystemUpdateEnrolls;

		void SortSystemUpdateEnrolls(SimulationStage stage);
		void RemoveSystem(System* system);

		System* CreateSystemFromName(const std::string& systemTypeName);
		
		template <typename tnSystem>
		System* CreateSystemAsBase() { return CreateSystem<tnSystem>(); }
	};
	
}