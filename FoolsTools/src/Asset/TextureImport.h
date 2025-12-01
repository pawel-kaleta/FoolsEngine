#pragma once

#include <filesystem>

#include <FoolsEngine.h>

namespace fe
{
	struct ImportData;

	namespace Description::Texture { struct Specification; }

	namespace TextureImport
	{
		void RenderWindow(ImportData* importData);

		void InitImport(ImportData* importData);

		struct Data
		{
			Description::Texture::Specification Specification;
		};
	};
}