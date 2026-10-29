#pragma once

#include "Context.h"
#include "Data.h"
#include "Resource.h"
#include "Pipeline.h"


namespace fe::Render::GAPI
{
	namespace Descriptors
	{
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

				StencilCases& Init()
				{
					new (this) StencilCases();
					return *this;
				}
			};

			struct Specification
			{
				DepthTestType	mDepthTestType = DepthTestType::None;
				StencilTestType	mStencilTestType = StencilTestType::None;
				StencilCases	mStencilCases;
				bool			mDepthWrite = true;

				Specification& Init()
				{
					new (this) Specification();
					return *this;
				}
			};
		}

		namespace Blend
		{
			FE_DECLARE_ENUM(BlendFunction, None, SourceAlpha, OneMinusSourceAlpha);

			struct Specification
			{
				BlendFunction mBlendFunction = BlendFunction::None;

				Specification& Init()
				{
					new (this) Specification();
					return *this;
				}
			};
		}
	}


	void	BeginRenderpass(GID framebuffer);

	void	ClearColor(UInt attachmentIndex, float value);
	void	ClearColor(UInt attachmentIndex, SInt value);
	void	ClearColor(UInt attachmentIndex, UInt value);
	void	ClearDepth(float value);
	void	ClearStencil(SInt value);
	void	ClearDepthStencil(float depthValue, SInt stencilValue);

	void	SetPipelineCmd(GID pipeline);
	void	SetDepthStencilState(const Descriptors::DepthStencil::Specification& depthStencil);
	void	SetBlendState(const Descriptors::Blend::Specification& blend);

	void	SetRootSource(GID source);
	void	SetIndexSource(GID source);
	void	SetDrawParamsSource(GID source);

	void	SetUniformSource(UInt bindingIndex, GID source, U32 offset, U32 size);
	void	SetBufferSource(UInt bindingIndex, GID source);

	void	DrawCmd(UInt primitiveCount, UInt indicesOffset, U32 rootOffset);
	void	DrawIndirectCmd(UInt paramsOffset, U32 rootOffset);
	void	MultiDrawIndirectCmd(UInt paramsOffset, UInt drawCount, U32 rootOffset);
	void	MultiDrawIndirectCountCmd(UInt paramsOffset, UInt maxDrawCount, U32 rootOffset);
}