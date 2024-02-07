#pragma once

namespace fe
{
	namespace TextureData
	{
		enum class Type
		{
			None = 0,
			Texture2D
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
			Specification(Components format, Format dataFormat)
				: Components(format), Format(dataFormat) {};

			Components Components = Components::None;
			Format     Format     = Format::None;
		};
	}
}