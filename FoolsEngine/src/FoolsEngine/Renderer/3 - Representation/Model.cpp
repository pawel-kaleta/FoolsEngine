#include "FE_pch.h"
#include "Model.h"

namespace fe
{
	void ModelUser::PlaceCoreComponent() const
	{
		Emplace<ACModelData>();
	}
}