#pragma once

#include <filesystem>

#include <FoolsEngine.h>

namespace fe
{
	struct ImportData;

	namespace TextureImport
	{
		void RenderWindow(ImportData* importData);

		void InitImport(ImportData* importData);

		struct Data
		{
			Render::GAPI::Descriptors::TextureSpec Spec;
			//Description::Texture::Archetype Archetype;
			//uint32_t ArchetypeID;
		};
	};
}