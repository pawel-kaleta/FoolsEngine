#pragma once

namespace fe
{
	using RenderTextureSlotID = uint32_t;

	namespace Description::Texture
	{
		FE_DECLARE_ENUM(Type, None, Texture2D);

#define FE_TEXTURE_DATA_USAGE RenderTarget_Depth, RenderTarget_Color, Map_BaseColor, Map_Roughness, Map_Metalness, Map_Normal, Map_AO, Map_ORM
		FE_DECLARE_ENUM(Usage, None, FE_TEXTURE_DATA_USAGE);

		FE_DECLARE_ENUM(Components, None, R, RG, RGB, RGBA, Depth, Stencil, DepthStencil);

		FE_DECLARE_ENUM(Format, None, R_8, RG_8, RGB_8, RGBA_8, R_UINT_32, DEPTH24STENCIL8);

		FE_DECLARE_ENUM(Wrapping, None, Repeat, MirrorRepeat, Clamp, Border);

		FE_DECLARE_ENUM(Filtering, None, Nearest, Bilinear);

		FE_DECLARE_ENUM(Mipmapping, None, Nearest, Liniear);

		FE_DECLARE_ENUM(AnisotropicFiltering, None, x2, x4, x8, x16);


		struct Specification
		{
			Specification() :
				Type(Type::None),
				Format(Format::None),
				Wrapping(Wrapping::None),
				Filtering(Filtering::None),
				Mipmapping(Mipmapping::None),
				AnisotropicFiltering(AnisotropicFiltering::None)
			{ }

			Type Type;
			Format Format;
			Wrapping Wrapping;
			Filtering Filtering;
			Mipmapping Mipmapping;
			AnisotropicFiltering AnisotropicFiltering;
		};

		constexpr Components ComponentsInFormat(Format format)
		{
			switch (format)
			{
			case Format::None:				return Components::None;
			case Format::R_8:				return Components::R;
			case Format::RG_8:				return Components::RG;
			case Format::RGB_8:				return Components::RGB;
			case Format::RGBA_8:			return Components::RGBA;
			case Format::R_UINT_32:			return Components::R;
			case Format::DEPTH24STENCIL8:	return Components::DepthStencil;
			default:
				FE_CORE_ASSERT(false, "Unrecognized Texture Format");
				return Components::None;
			}
		}
	}
}