#pragma once

#include <filesystem>

#include <FoolsEngine.h>

namespace fe
{
	struct ImportData;

	namespace TextureData { struct Specification; }

	namespace TextureImport
	{
		void RenderWindow(ImportData* importData);

		void InitImport(ImportData* importData);

		struct Data
		{
			TextureData::Specification Specification;
		};
	};
}