#include "FE_pch.h"
#include "Model.h"

namespace fe
{
	void Model::PlaceCoreComponent()
	{
		Emplace<ACModelData>();
	}
}