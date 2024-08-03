#pragma once

#include <glm/glm.hpp>

namespace fe
{
	using RenderTextureSlotID = uint32_t;

	namespace TextureData
	{
		enum class Type
		{
			None = 0,
			Texture2D
		};
		
		enum class Usage
		{
			None = 0,
			RenderTarget_Depth,
			RenderTarget_Color,
			Map_Albedo,
			Map_Roughness,
			Map_Metalness,
			Map_Normal,
			Map_AO
		};

		enum class Components
		{
			None = 0,

			R    = 1,
			RG   = 2,
			RGB  = 3,
			RGBA = 4,

			Depth,
			Stencil,
			DepthStencil
		};

		enum class Format
		{
			None = 0,

			R_8,
			RG_8,
			RGB_8,
			RGBA_8,

			R_UINT_32,
			
			DEPTH24STENCIL8
		};

		struct Specification
		{
			Usage		Usage		= Usage::None;
			Components	Components	= Components::None;
			Format		Format		= Format::None;
			uint32_t	Width		= 0;
			uint32_t	Height		= 0;
		};
	}
}