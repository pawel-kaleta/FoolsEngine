#pragma once

#include <filesystem>

#include <FoolsEngine.h>

namespace fe
{
	struct ImportData;

	namespace TextureData { struct Specification; }

	namespace TextureImport
	{
		void RenderWindow(ImportData* const importData);

		void InitImport(ImportData* const importData);

		struct Data
		{
			TextureData::Specification Specification;
		};
	};
}