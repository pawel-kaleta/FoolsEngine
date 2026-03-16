#pragma once

namespace fe
{
	namespace Description::Pipeline
	{
		FE_DECLARE_ENUM(DepthTestType, None, Never, Always, NotEqual, Less, LessEqual, Equal, GreaterEqual, Greater);

		FE_DECLARE_ENUM(BlendFunction, None, SourceAlpha, OneMinusSourceAlpha);
	}
}