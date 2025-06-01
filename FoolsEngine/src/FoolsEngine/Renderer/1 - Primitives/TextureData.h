#pragma once

#include "FoolsEngine\Utils\DeclareEnum.h"

namespace fe
{
	using RenderTextureSlotID = uint32_t;

	namespace TextureData
	{
		FE_DECLARE_ENUM(Type, None, Texture2D);

		FE_DECLARE_ENUM(Usage, None, RenderTarget_Depth, RenderTarget_Color, Map_Albedo, Map_Roughness, Map_Metalness, Map_Normal, Map_AO);

		FE_DECLARE_ENUM(Components, None, R, RG, RGB, RGBA, Depth, Stencil, DepthStencil);

		FE_DECLARE_ENUM(Format, None, R_8, RG_8, RGB_8, RGBA_8, R_UINT_32, DEPTH24STENCIL8);

		struct Specification
		{
			Usage	    Usage;
			Components  Components;
			Format		Format;
			uint32_t	Width;
			uint32_t	Height;

			void Init()
			{
				Usage      = Usage::None;
				Components = Components::None;
				Format     = Format::None;
				Width      = 0;
				Height     = 0;
			}
		};
	}
}