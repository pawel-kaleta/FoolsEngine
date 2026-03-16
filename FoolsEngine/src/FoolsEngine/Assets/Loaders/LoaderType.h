#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"

namespace fe
{
	FE_DECLARE_ENUM(LoaderType,
		Texture,
		Shader,
		Geometry,

		Count,
		None
	);
}