#include "FE_pch.h"
#include "ComponentTypesRegistry.h"

#include "Component.h"
#include "Components/2DComponents.h"
#include "Components/MeshComponents.h"
#include "Components/LightComponents.h"
#include "Components/RenderingComponents.h"

namespace fe
{
	ComponentTypesRegistry* ComponentTypesRegistry::s_Instance;

	void ComponentTypesRegistry::RegisterComponents()
	{
		FE_PROFILER_FUNC();

#define _REGISTER_DATA_COMPONENT_CALL(x) RegisterDataComponent<x>();

		FE_FOR_EACH(_REGISTER_DATA_COMPONENT_CALL, FE_COMPONENT_TYPES_LIST);
	}
}