#include "Components.h"

#include "FoolsEngine/Scene/Component.h"
#include "FoolsEngine/Scene/Components/2DComponents.h"
#include "FoolsEngine/Scene/Components/MeshComponents.h"
#include "FoolsEngine/Scene/Components/LightComponents.h"
#include "FoolsEngine/Scene/Components/RenderingComponents.h"

namespace fe::TypeRegs
{
	DynamicArena<Components::DataComponentRegistryItem> Components::sDataItems;
	DynamicArena<Components::FlagComponentRegistryItem> Components::sFlagItems;

	void Components::Init()
	{
		sDataItems.Init();
		sFlagItems.Init();
	}

	void Components::RegisterComponents()
	{
		FE_PROFILER_FUNC();

#define _REGISTER_DATA_COMPONENT_CALL(x) RegisterDataComponent<x>();

		FE_FOR_EACH(_REGISTER_DATA_COMPONENT_CALL, FE_COMPONENT_TYPES_LIST);
	}
}