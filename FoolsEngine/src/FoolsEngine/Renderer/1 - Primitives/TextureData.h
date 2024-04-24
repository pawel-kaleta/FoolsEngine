#pragma once

#include <glm/glm.hpp>

namespace fe
{
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

			R_F,
			R_UI,
			RGB_F,
			RGBA_F,

			DepthStencil
		};

		enum class Format
		{
			None = 0,

			R_FLOAT_32,
			R_UINT_32,
			RGB_FLOAT_8,
			RGBA_FLOAT_8,
			
			DEPTH24STENCIL8
		};

		struct Specification
		{
			Specification() = default;
			Specification(Type type, Usage usage, Components components, Format format)
				: Type(type), Usage(usage), Components(components), Format(format) {};

			Type       Type       = Type::None;
			Usage      Usage      = Usage::None;
			Components Components = Components::None;
			Format     Format     = Format::None;
			uint32_t   Width      = 0;
			uint32_t   Height     = 0;
		};
	}
}