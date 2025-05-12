#include "FE_pch.h"
#include "Model.h"

namespace fe
{
	void Model::PlaceCoreComponents()
	{
		Emplace< ACModelSpecification>().Init();
	}

	void Model::Release()
	{

	}

	void Model::SendDataToGPU(GDIType GDI, void* data)
	{

	}

	void Model::UnloadFromCPU()
	{

	}
}