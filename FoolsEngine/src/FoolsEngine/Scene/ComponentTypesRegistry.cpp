#include "FE_pch.h"
#include "ComponentTypesRegistry.h"

#include "Component.h"
#include "Components/RenderingComponents.h"
#include "Components/2DComponents.h"
#include "Components/MeshComponents.h"

namespace fe
{
	ComponentTypesRegistry* ComponentTypesRegistry::s_Instance;

	void ComponentTypesRegistry::RegisterComponents()
	{
		FE_PROFILER_FUNC();

		RegisterDataComponent<CCamera>();

		RegisterDataComponent<CTile>();
		RegisterDataComponent<CSprite>();

		RegisterDataComponent<CRenderMesh>();
		RegisterDataComponent<CRenderMeshView>();
		RegisterDataComponent<CModel>();
		RegisterDataComponent<CModelView>();
	}

}