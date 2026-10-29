#pragma once

#include "FoolsEngine/Foundation/Containers/Arena.h"

#include "Context.h"
#include "Data.h"
#include "Resource.h"

namespace fe::Render::GAPI
{
	namespace Raster
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

	namespace DepthStencil
	{
		FE_DECLARE_ENUM(DepthTestType, None, Never, Always, NotEqual, Less, LessEqual, Equal, GreaterEqual, Greater);

		FE_DECLARE_ENUM(StencilTestType, None, Never, Always, NotEqual, Less, LessEqual, Equal, GreaterEqual, Greater);

		FE_DECLARE_ENUM(StencilOperation, None, Keep, Zero, Replace, IncrementCap, IncrementWrap, DecrementCap, DecrementWrap, Invert);

		struct StencilCases
		{
			StencilOperation Fail = StencilOperation::None;
			StencilOperation PassDepthFail = StencilOperation::None;
			StencilOperation PassDepthPass = StencilOperation::None;
		};

		struct Specification
		{
			DepthTestType	mDepthTestType = DepthTestType::None;
			StencilTestType	mStencilTestType = StencilTestType::None;
			StencilCases	mStencilCases;
			bool			mDepthWrite = true;
		};
	}

	namespace Blend
	{
		FE_DECLARE_ENUM(BlendFunction, None, SourceAlpha, OneMinusSourceAlpha);

		struct Specification
		{
			BlendFunction	mBlendFunction = BlendFunction::None;
		};
	}

	GID		CreateGraphicsPipelineCmd(GID vertexShader, GID fragmentShader, const Raster::Specification& spec);
	void	DestroyPipelineCmd(GID pipeline);

	void	SetFramebuffer(GID framebuffer);
	void	SetPipelineCmd(GID pipeline);
	void	SetDepthStencilState(const DepthStencil::Specification& depthStencil);
	void	SetBlendState(const Blend::Specification& blend);
	void	SetBufferBinding(GID buffer, UInt bindingIndex);
	void	SetIndexSource(GID source);
	void	SetDrawParamsSource(GID source);

	void	DrawCmd(UInt primitiveCount, UInt indicesOffset, U32 rootDataIndex);
	void	DrawIndirectCmd(UInt paramsOffset, U32 rootDataIndex);
	void	MultiDrawIndirectCmd(UInt paramsOffset, UInt drawCount, U32 rootDataIndex);
	void	MultiDrawIndirectCountCmd(UInt paramsOffset, UInt maxDrawCount, U32 rootDataIndex);

	void	ClearColor(UInt attachmentIndex, float value);
	void	ClearColor(UInt attachmentIndex, SInt value);
	void	ClearColor(UInt attachmentIndex, UInt value);
	void	ClearDepth(float value);
	void	ClearStencil(SInt value);
	void	ClearDepthStencil(float depthValue, SInt stencilValue);
}