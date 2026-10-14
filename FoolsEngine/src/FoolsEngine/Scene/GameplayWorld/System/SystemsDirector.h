#pragma once

#include "System.h"

#include "FoolsEngine/Scene/SimulationStage.h"
#include "FoolsEngine/Foundation/Memory/Arena.h"
#include "FoolsEngine/Foundation/Memory/Splice.h"
#include "FoolsEngine/Foundation/Memory/String.h"
#include "FoolsEngine/Foundation/Utils/TypeSetup.h"


namespace fe
{
	class UUID;

	class SystemsDirector
	{
	public:
		SystemsDirector() = default;
		SystemsDirector(const SystemsDirector& other) = delete;

		struct SystemUpdateEnroll
		{
			FE_TYPE_SETUP(SystemUpdateEnroll);

			System* System;
			void (System::* OnUpdateFuncPtr)();
			uint32_t Priority;
		};

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
			m_Systems.Append((System*)system);
			
			system->m_SystemsDirector = this;
			system->OnInitialize();

			return system;
		}

		System* GetSystemFromName(String name) const;
		System* GetSystemFromUUID(UUID uuid) const;

		DynamicArena<System*> m_Systems;
		Array<DynamicArena<SystemUpdateEnroll>, SimulationStage::Count> m_SystemUpdateEnrolls;
	private:
		friend class SystemsInspector;
		friend class SystemTypesRegistry;
		friend class SceneSerializerYAML;

		void RemoveSystem(System* system);

		System* CreateSystemFromName(String systemTypeName);
		
		template <typename tnSystem>
		System* CreateSystemAsBase() { return CreateSystem<tnSystem>(); }
	};
	
}