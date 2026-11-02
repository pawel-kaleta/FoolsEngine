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
				U08				mStencilTestMask = 0b11111111;
				U08				mStencilWriteMask = 0b11111111;
				U08				mStencilTestValue = 0b11111111;
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

	void	BeginRenderpassCmd(GID framebuffer);

	void	ClearColorCmd(UInt attachmentIndex, Descriptors::Raster::ColorMask mask, Array<F32, 4>& value);
	void	ClearColorCmd(UInt attachmentIndex, Descriptors::Raster::ColorMask mask, Array<S32, 4>& value);
	void	ClearColorCmd(UInt attachmentIndex, Descriptors::Raster::ColorMask mask, Array<U32, 4>& value);
	void	ClearDepthCmd(float value);
	void	ClearStencilCmd(U08 mask, U08 value);

	void	SetPipelineCmd(GID pipeline);
	void	SetDepthStencilStateCmd(const Descriptors::DepthStencil::Specification& depthStencil);
	void	SetBlendStateCmd(const Descriptors::Blend::Specification& blend);

	void	SetRootSourceCmd(GID source);
	void	SetIndexSourceCmd(GID source);
	void	SetDrawParamsSourceCmd(GID source);

	void	SetUniformSourceCmd(UInt bindingIndex, GID source, U32 offset, U32 size);
	void	SetBufferSourceCmd(UInt bindingIndex, GID source);

	void	DrawCmd(UInt primitiveCount, UInt indicesOffset, U32 rootOffset);
	void	DrawIndirectCmd(UInt paramsOffset, U32 rootOffset);
	void	MultiDrawIndirectCmd(UInt paramsOffset, UInt drawCount, U32 rootOffset);
	void	MultiDrawIndirectCountCmd(UInt paramsOffset, UInt maxDrawCount, U32 rootOffset);

	void	EndRenderpass();
}