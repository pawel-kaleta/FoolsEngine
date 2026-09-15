#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Foundation/Memory/Splice.h"

#include <glm/glm.hpp>

namespace fe::GAPI::Descriptors
{
	namespace Texture
	{
		FE_DECLARE_ENUM(Type, None, Texture1D, Texture2D, Texture3D);

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

	namespace Data
	{
		struct test
		{
			bool x, y;
		};

		using std140_bool = U32;
		struct alignas( 8) std140_bvec2 : fe::Array< std140_bool, 2 > {};
		struct alignas(16) std140_bvec3 : fe::Array< std140_bool, 3 > {};
		struct alignas(16) std140_bvec4 : fe::Array< std140_bool, 4 > {};

		using std140_int = S32;
		struct alignas( 8) std140_ivec2 : fe::Array< std140_int, 2 > {};
		struct alignas(16) std140_ivec3 : fe::Array< std140_int, 3 > {};
		struct alignas(16) std140_ivec4 : fe::Array< std140_int, 4 > {};

		using std140_uint = U32;
		struct alignas( 8) std140_uvec2 : fe::Array< std140_uint, 2 > {};
		struct alignas(16) std140_uvec3 : fe::Array< std140_uint, 3 > {};
		struct alignas(16) std140_uvec4 : fe::Array< std140_uint, 4 > {};

		using std140_float = F32;
		struct alignas( 8) std140_vec2 : fe::Array< std140_float, 2 > {};
		struct alignas(16) std140_vec3 : fe::Array< std140_float, 3 > {};
		struct alignas(16) std140_vec4 : fe::Array< std140_float, 4 > {};

		struct std140_mat2x2 { std140_vec2 data[2]; };
		struct std140_mat2x3 { std140_vec3 data[2]; };
		struct std140_mat2x4 { std140_vec4 data[2]; };
		struct std140_mat3x2 { std140_vec2 data[3]; };
		struct std140_mat3x3 { std140_vec3 data[3]; };
		struct std140_mat3x4 { std140_vec4 data[3]; };
		struct std140_mat4x2 { std140_vec2 data[4]; };
		struct std140_mat4x3 { std140_vec3 data[4]; };
		struct std140_mat4x4 { std140_vec4 data[4]; };

		template <typename T>
		struct std140_Array
		{
			struct __declspec(align(16)) ElementType : T { };


		};

		using std140_double = F64;
		struct alignas(16) std140_vec2 : fe::Array< std140_double, 2 > {};
		struct alignas(32) std140_vec3 : fe::Array< std140_double, 3 > {};
		struct alignas(32) std140_vec4 : fe::Array< std140_double, 4 > {};
		


	}
}