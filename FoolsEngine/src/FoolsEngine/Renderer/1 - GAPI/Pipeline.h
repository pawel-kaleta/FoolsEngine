#pragma once

#include "FoolsEngine/Foundation/Containers/Arena.h"

#include "Context.h"
#include "Data.h"
#include "Resource.h"

namespace fe::Render::GAPI
{
	namespace Descriptors::Raster
	{
		FE_DECLARE_ENUM(PrimitiveType, None, Point, Line, Triangle);

		FE_DECLARE_ENUM(FaceCullTest, None, Never, Back, Front, Always);

		struct Specification
		{
			PrimitiveType					mPrimitiveType = PrimitiveType::None;
			FaceCullTest					mFaceCullTest = FaceCullTest::None;
			Descriptors::FramebufferSpec	mFramebufferSpec;
			ArrayArena<U08, 6>				mColorWriteMasks;

			Specification& Init()
			{
				mPrimitiveType = PrimitiveType::None;
				mFaceCullTest = FaceCullTest::None;
				mFramebufferSpec.Init();
				std::memset(&mColorWriteMasks[0], 0b1111, 6);
				return *this;
			}
		};
	}

	GID		CreateGraphicsPipelineCmd(GID vertexShader, GID fragmentShader, const Descriptors::Raster::Specification& spec);
	void	DestroyPipelineCmd(GID pipeline);
}