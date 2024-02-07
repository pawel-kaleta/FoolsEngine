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

			RGB,
			RGBA,

			DepthStencil
		};

		enum class Format
		{
			None = 0,

			RGB8,
			RGBA8,
			
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