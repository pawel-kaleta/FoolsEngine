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

		struct ColorMask
		{
			bool red, green, blue, alpha;

			void operator=(std::initializer_list<bool> list)
			{
				Init();
				std::memcpy(&red, list.begin(), list.size()*sizeof(bool));
			}

			ColorMask& Init()
			{
				red = 1; green = 1; blue = 1; alpha = 1;
				return *this;
			}
		};

		struct Specification
		{
			PrimitiveType					mPrimitiveType = PrimitiveType::None;
			FaceCullTest					mFaceCullTest = FaceCullTest::None;
			Descriptors::FramebufferSpec	mFramebufferSpec;
			ArrayArena<ColorMask, 6>		mColorWriteMasks = {};

			Specification& Init()
			{
				mPrimitiveType = PrimitiveType::None;
				mFaceCullTest = FaceCullTest::None;
				mFramebufferSpec.Init();
				mColorWriteMasks.Count = 0;
				std::memset(mColorWriteMasks.begin(), 1, 6 * sizeof(ColorMask));
				return *this;
			}
		};
	}

	GID		CreateGraphicsPipelineCmd(GID vertexShader, GID fragmentShader, const Descriptors::Raster::Specification& spec, UInt rootDataSize);
	void	DestroyPipelineCmd(GID pipeline);
}