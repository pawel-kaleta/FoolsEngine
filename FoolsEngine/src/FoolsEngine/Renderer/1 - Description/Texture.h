#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Application/UUID.h"

#include <glm/glm.hpp>

namespace fe
{
	using RenderTextureSlotID = U32;

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
		
		constexpr bool IsDepthOrStencil(Format format)
		{
			auto components = ComponentsInFormat(format);
			if (components == Components::Depth || components == Components::Stencil || components == Components::DepthStencil)
				return true;
			return false;
		}

		constexpr UInt CountOfComponents(Components components)
		{
			switch (components)
			{
			case Components::None:
				FE_CORE_ASSERT(false, "Texture Components None does not have count");
				return 0;
			case Components::R:				return 1;
			case Components::RG:			return 2;
			case Components::RGB:			return 3;
			case Components::RGBA:			return 4;
			case Components::Depth:			return 1;
			case Components::Stencil:		return 1;
			case Components::DepthStencil:
				FE_CORE_ASSERT(false, "Unequal components");
				return 0;
			default:
				FE_CORE_ASSERT(false, "Unrecognized Texture Components");
				return 0;
			}
		}

		constexpr UInt ComponentsCountInFormat(Format format)
		{
			switch (format)
			{
			case Format::None:
				FE_CORE_ASSERT(false, "Texture Format None does not have components");
				return 0;
			case Format::R_8:				return 1;
			case Format::RG_8:				return 2;
			case Format::RGB_8:				return 3;
			case Format::RGBA_8:			return 4;
			case Format::R_UINT_32:			return 1;
			case Format::DEPTH24STENCIL8:
				FE_CORE_ASSERT(false, "Texture Format have unequal components");
				return 0;
			default:
				FE_CORE_ASSERT(false, "Unrecognized Texture Format");
				return 0;
			}
		}

		constexpr UInt SizeOfFormat(Format format)
		{
			switch (format)
			{
			case Format::None:
				FE_CORE_ASSERT(false, "Texture Format None does not have a size");
				return 0;
			case Format::R_8:				return 1;
			case Format::RG_8:				return 2;
			case Format::RGB_8:				return 3;
			case Format::RGBA_8:			return 4;
			case Format::R_UINT_32:			return 4;
			case Format::DEPTH24STENCIL8:	return 4;
			default:
				FE_CORE_ASSERT(false, "Unrecognized Texture Format");
				return 0;
			}
		}

		struct Archetype
		{
			UUID UUID;
			Type Type = Type::None;
			Format Format = Format::None;
			Wrapping Wrapping = Wrapping::None;
			Filtering Filtering = Filtering::None;
			Mipmapping Mipmapping = Mipmapping::None;
			AnisotropicFiltering AnisotropicFiltering = AnisotropicFiltering::None;
		};

		struct Specification
		{
			U32 ArchetypeID = -1;
			Usage Usage = Usage::None;
			U32 Width = 0;
			U32 Height = 0;
			glm::vec3 BorderColor;
		};
	}
}