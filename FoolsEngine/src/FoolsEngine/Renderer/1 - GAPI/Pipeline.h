#pragma once

#include "GAPI.h"
#include "Data.h"
#include "Resource.h"

namespace fe::GAPI::Pipeline
{
	namespace Raster
	{
		FE_DECLARE_ENUM(PrimitiveType, None, Point, Line, Triangle);

		FE_DECLARE_ENUM(FaceCullTest, None, Never, Back, Front, Always);

		struct Attachment
		{
			Resource::Texture::Format Format = Resource::Texture::Format::None;
			U08 mWriteMask = 0b1111;
		};

		struct Specification
		{
			PrimitiveType				mPrimitiveType = PrimitiveType::None;
			FaceCullTest				mFaceCullTest = FaceCullTest::None;
			ArrayArena<Attachment, 6>	mColorAttachments;
			Resource::Texture::Format	mDepthStencilFormat = Resource::Texture::Format::None;
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

	void	SetColorAttachment(GID pipeline, GID texture, UInt index);
	void	SetDepthStencilAttachment(GID pipeline, GID texture);
	void	SetDepthStencilSpec(GID pipeline, const DepthStencil::Specification& depthStencil);
	void	SetBlendSpec(GID pipeline, const Blend::Specification& blend);
	void	SetUBO(GID pipeline, GID stream, UInt bindingIndex);
	void	SetSSBO(GID pipeline, GID buffer, UInt bindingIndex);
	// buffer or stream
	void	SetIndexSource(GID pipeline, GID source);
	// buffer or stream
	void	SetDrawParamsSource(GID pipeline, GID source);

	void	ActivatePipelineCmd(GID pipeline);
	void	DrawCmd(UInt primitiveCount, UInt indicesOffset);
	void	DrawIndirectCmd(UInt paramsOffset);
	void	MultiDrawIndirectCmd(UInt paramsOffset, UInt drawCount);
	void	MultiDrawIndirectCountCmd(UInt paramsOffset, UInt maxDrawCount);
	void	DeactivatePipelineCmd();

	void	DestroyPipelineCmd(GID pipeline);
}