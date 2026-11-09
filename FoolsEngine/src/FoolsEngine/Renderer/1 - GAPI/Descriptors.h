#pragma once

#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Foundation/Containers/Arena.h"
#include "FoolsEngine/Foundation/Containers/String.h"

#include "Context.h"
#include "Data.h"

#include <glm/glm.hpp>

namespace fe::Render::GAPI::Descriptors
{
	FE_DECLARE_ENUM(ShaderType, None, Vertex, Fragment, Compute)

	namespace Texture
	{
		FE_DECLARE_ENUM(Type, None, Texture1D, Texture2D, Texture3D, Texture2DArray)

		FE_DECLARE_ENUM(Format, None, R_8, RG_8, RGB_8, RGBA_8, R_UINT_32, DEPTH32F, DEPTH24_STENCIL8, DEPTH32F_STENCIL8)

		struct Spec
		{
			Type mType = Type::None;
			Format mFormat = Format::None;
			glm::vec<3, U32> mDimentions = { 0, 0, 0 };
			U32 mMipCount = 1;
			U32 mLayerCount = 1;

			void CalculateMipCount()
			{
				mMipCount = glm::log2((float)glm::max(mDimentions.x, mDimentions.y));
			}

			// UsageFlags mUsageFlags = UsageFlags::None;  ?? Sampled, Storage, ColorAttachment, DepthStencilAttachment

			Spec& Init()
			{
				mType = Type::None;
				mFormat = Format::None;
				mDimentions = { 0, 0, 1 };
				mMipCount = 1;
				mLayerCount = 1;
				return *this;
			}
		};

		namespace Sampling
		{
			FE_DECLARE_ENUM(Wrapping, None, Repeat, MirrorRepeat, Clamp, Border)

			FE_DECLARE_ENUM(Filtering, None, Nearest, Bilinear)

			FE_DECLARE_ENUM(Mipmapping, None, Nearest, Liniear)

			FE_DECLARE_ENUM(ASFiltering, None, x2, x4, x8, x16)

			struct ViewSpec
			{
				Wrapping	mWrapping = Wrapping::None;
				Filtering	mFiltering = Filtering::None;
				Mipmapping	mMipmapping = Mipmapping::None;
				ASFiltering	mASFiltering = ASFiltering::None;
				glm::vec4	mBorderColor = { 0.0, 0.0, 0.0, 1.0 };

				void Init() { new (this) ViewSpec(); }
			};
		}
	}

	namespace Raster
	{
		FE_DECLARE_ENUM(DepthRange, None, MinusOneToOne, ZeroToOne);

		struct FramebufferSpec
		{
			ArrayArena<Texture::Format, 6>	mColorFormats = {};
			Texture::Format					mDepthStencilFormat = Texture::Format::None;
			DepthRange						mDepthRange = DepthRange::None;

			FramebufferSpec& Init()
			{
				mColorFormats.Init();
				mDepthStencilFormat = Texture::Format::None;
				mDepthRange = DepthRange::None;
				return *this;
			}
		};

		FE_DECLARE_ENUM(PrimitiveType, None, Point, Line, Triangle);

		FE_DECLARE_ENUM(FaceCulling, None, Never, Back, Front, Always);

		struct ColorMask
		{
			U08 Val = 0b1111;

			bool Red()		const { return Val & 0b00001000; }
			bool Green()	const { return Val & 0b00000100; }
			bool Blue()		const { return Val & 0b00000010; }
			bool Alpha()	const { return Val & 0b00000001; }

			void Red(bool flag)		{ Val = flag * (Val | 0b00001000) + !flag * (Val & ~0b00001000); }
			void Green(bool flag)	{ Val = flag * (Val | 0b00000100) + !flag * (Val & ~0b00000100); }
			void Blue(bool flag)	{ Val = flag * (Val | 0b00000010) + !flag * (Val & ~0b00000010); }
			void Alpha(bool flag)	{ Val = flag * (Val | 0b00000001) + !flag * (Val & ~0b00000001); }

			ColorMask& Init(U08 val = 0b1111)
			{
				Val = val;
				return *this;
			}
		};

		struct Spec
		{
			PrimitiveType				mPrimitiveType = PrimitiveType::None;
			FaceCulling					mFaceCullTest = FaceCulling::None;
			FramebufferSpec				mFramebufferSpec;
			ArrayArena<ColorMask, 6>	mColorWriteMasks = {};

			Spec& Init()
			{
				mPrimitiveType = PrimitiveType::None;
				mFaceCullTest = FaceCulling::None;
				mFramebufferSpec.Init();
				mColorWriteMasks.Count = 0;
				std::memset(mColorWriteMasks.begin(), 1, 6 * sizeof(ColorMask));
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

			StencilCases& Init()
			{
				new (this) StencilCases();
				return *this;
			}
		};

		struct Spec
		{
			DepthTestType	mDepthTestType = DepthTestType::None;
			StencilTestType	mStencilTestType = StencilTestType::None;
			StencilCases	mStencilCases;
			U08				mStencilTestMask = 0b11111111;
			U08				mStencilWriteMask = 0b11111111;
			U08				mStencilTestValue = 0b11111111;
			bool			mDepthWrite = true;

			Spec& Init()
			{
				new (this) Spec();
				return *this;
			}
		};
	}

	namespace Blend
	{
		FE_DECLARE_ENUM(BlendFunction, None, SourceAlpha, OneMinusSourceAlpha);

		struct Spec
		{
			BlendFunction mBlendFunction = BlendFunction::None;

			Spec& Init()
			{
				new (this) Spec();
				return *this;
			}
		};
	}
}