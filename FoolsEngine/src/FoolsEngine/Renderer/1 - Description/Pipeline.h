#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Application/UUID.h"

namespace fe
{
	namespace Description::Pipeline
	{
		FE_DECLARE_ENUM(PrimitiveType, None, Point, Line, Triangle);

		FE_DECLARE_ENUM(FaceCullTest, None, Never, Back, Front, Always);

		FE_DECLARE_ENUM(DepthTestType, None, Never, Always, NotEqual, Less, LessEqual, Equal, GreaterEqual, Greater);

		FE_DECLARE_ENUM(StencilTestType, None, Never, Always, NotEqual, Less, LessEqual, Equal, GreaterEqual, Greater);

		FE_DECLARE_ENUM(StencilOperation, None, Keep, Zero, Replace, IncrementCap, IncrementWrap, DecrementCap, DecrementWrap, Invert);

		struct StencilCases
		{
			StencilOperation Fail			= StencilOperation::None;
			StencilOperation PassDepthFail	= StencilOperation::None;
			StencilOperation PassDepthPass	= StencilOperation::None;
		};

		FE_DECLARE_ENUM(BlendFunction, None, SourceAlpha, OneMinusSourceAlpha);

		struct RasterState
		{
			UUID UUID = fe::UUID();

			PrimitiveType		mPrimitiveType = PrimitiveType::None;
			FaceCullTest		mFaceCullTest = FaceCullTest::None;

			fe::UUID FramebufferSpec = 0;
		};

		struct DepthStencilState
		{
			DepthTestType		mDepthTestType = DepthTestType::None;
			StencilTestType		mStencilTestType = StencilTestType::None;
			StencilCases		mStencilCases;
		};
	}
}