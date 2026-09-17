#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Foundation/Memory/Splice.h"

#include <glm/glm.hpp>

namespace fe::GAPI::Descriptors
{
	namespace Texture
	{
		FE_DECLARE_ENUM(Type, None, Texture1D, Texture2D, Texture3D, Texture2DArray);

		FE_DECLARE_ENUM(Format, None, R_8, RG_8, RGB_8, RGBA_8, R_UINT_32, DEPTH24STENCIL8);

		struct Specification
		{
			Type mType = Type::None;
			Format mFormat = Format::None;
			glm::vec<3, U32> mDimentions = { 0, 0, 0 };
			U32 mMipCount = 1;
			U32 mLayerCount = 1;
			glm::vec4 mBorderColor = { 0.0, 0.0, 0.0, 1.0 };

			// UsageFlags mUsageFlags = UsageFlags::None;  ?? Sampled, Storage, ColorAttachment, DepthStencilAttachment
		};

		FE_DECLARE_ENUM(Wrapping, None, Repeat, MirrorRepeat, Clamp, Border);

		FE_DECLARE_ENUM(Filtering, None, Nearest, Bilinear);

		FE_DECLARE_ENUM(Mipmapping, None, Nearest, Liniear);

		FE_DECLARE_ENUM(ASFiltering, None, x2, x4, x8, x16);

		struct Sampler
		{
			Wrapping	mWrapping = Wrapping::None;
			Filtering	mFiltering = Filtering::None;
			Mipmapping	mMipmapping = Mipmapping::None;
			ASFiltering	mASFiltering = ASFiltering::None;
		};
	}

	namespace Raster
	{
		FE_DECLARE_ENUM(PrimitiveType, None, Point, Line, Triangle);

		FE_DECLARE_ENUM(FaceCullTest, None, Never, Back, Front, Always);

		struct Attachment
		{
			Texture::Format Format = Texture::Format::None;
			U08 mWriteMask = 0b1111;
		};

		struct Specification
		{
			PrimitiveType		mPrimitiveType = PrimitiveType::None;
			FaceCullTest		mFaceCullTest = FaceCullTest::None;
			Splice<Attachment>	mColorAttachments;
			Texture::Format		mDepthStencilFormat = Texture::Format::None;
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

	namespace Shader
	{
		FE_DECLARE_ENUM(ShaderType, None, Vertex, Fragment, Compute);
	}
}