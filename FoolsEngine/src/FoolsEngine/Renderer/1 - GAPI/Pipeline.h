#pragma once

#include "FoolsEngine/Foundation/Memory/Arena.h"

#include "Context.h"
#include "Data.h"
#include "Resource.h"

namespace fe::Render::GAPI
{
	namespace Raster
	{
		FE_DECLARE_ENUM(PrimitiveType, None, Point, Line, Triangle);

		FE_DECLARE_ENUM(FaceCullTest, None, Never, Back, Front, Always);

		struct Attachment
		{
			Descriptors::TextureFormat Format = Descriptors::TextureFormat::None;
			U08 mWriteMask = 0b1111;
		};

		struct Specification
		{
			PrimitiveType				mPrimitiveType = PrimitiveType::None;
			FaceCullTest				mFaceCullTest = FaceCullTest::None;
			ArrayArena<Attachment, 6>	mColorAttachments;
			Descriptors::TextureFormat mDepthStencilFormat = Descriptors::TextureFormat::None;
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
			bool			mDepthTest = true;
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
	// buffer or downstream
	void	SetUBO(GID pipeline, GID source, UInt bindingIndex);
	// buffer, downstream or upstream
	void	SetSSBO(GID pipeline, GID buffer, UInt bindingIndex);
	// buffer or downstream
	void	SetIndexSource(GID pipeline, GID source);
	// buffer or downstream
	void	SetDrawParamsSource(GID pipeline, GID source);

	void	ActivatePipelineCmd(GID pipeline);
	void	DrawCmd(GID pipeline, UInt primitiveCount, UInt indicesOffset);
	void	DrawIndirectCmd(GID pipeline, UInt paramsOffset);
	void	MultiDrawIndirectCmd(GID pipeline, UInt paramsOffset, UInt drawCount);
	void	MultiDrawIndirectCountCmd(GID pipeline, UInt paramsOffset, UInt maxDrawCount);

	void	DestroyPipelineCmd(GID pipeline);
}