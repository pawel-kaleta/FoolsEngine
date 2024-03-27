#include "FE_pch.h"
#include "ComponentTypesRegistry.h"

#include "Component.h"

namespace fe
{
	ComponentTypesRegistry ComponentTypesRegistry::s_Registry = ComponentTypesRegistry();

	void ComponentTypesRegistry::RegisterComponents()
	{
		RegisterDataComponent<CCamera>();
		RegisterDataComponent<CTile>();
		RegisterDataComponent<CSprite>();
		RegisterDataComponent<CMesh>();
		RegisterDataComponent<CMaterialInstance>();
	}

}