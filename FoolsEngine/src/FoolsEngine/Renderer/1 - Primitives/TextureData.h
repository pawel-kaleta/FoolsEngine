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

		enum class Format
		{
			None = 0,

			RGB,
			RGBA,

			DepthStencil
		};

		enum class DataFormat
		{
			None = 0,

			RGB8,
			RGBA8,
			
			DEPTH24STENCIL8
		};

		struct Specification
		{
			Specification() = default;
			Specification(Format format, DataFormat dataFormat)
				: Format(format), DataFormat(dataFormat) {};

			Format     Format     = Format::None;
			DataFormat DataFormat = DataFormat::None;
		};
	}
}