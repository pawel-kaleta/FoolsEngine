#include "FE_pch.h"
#include "TextureLoader.h"

#include "FoolsEngine/Foundation/Memory/Scratchpad.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace fe
{
	void TextureLoader::LoadTexture(const std::filesystem::path& sourceFilePath, const AssetUser<Texture2D>& textureUser)
	{
		FE_PROFILER_FUNC();

		auto& data_location = textureUser.GetCoreComponent().Data;
		if (data_location)
			return;

		//TO DO: dont override specification, use import settings
		auto& spec = textureUser.GetCoreComponent().Specification;
		int width, height, channels;

		// TO DO: flipping should be happennig when uploding to gpu, not when loading from disk
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data;
		
		Scratchpad sp;
		std::pmr::string filename("stbi_load - " + sourceFilePath.filename().string<PMR_STRING_TEMPLATE_PARAMS>(), &sp);
		FE_PROFILER_SCOPE(filename.c_str());
		std::pmr::string file_path = sourceFilePath.string<PMR_STRING_TEMPLATE_PARAMS>();
		data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
		
		{
			FE_PROFILER_SCOPE("Archetype Init");

			data_location = data;

			FE_CORE_ASSERT(data, "Failed to load image!");
			spec.Width = width;
			spec.Height = height;
			auto& archetype = Description::Library::Get().TextureArchetypes[spec.ArchetypeID];

			switch (channels)
			{
			case 1:
				archetype.Format = Description::Texture::Format::R_8;
				return;
			case 3:
				archetype.Format = Description::Texture::Format::RGB_8;
				return;
			case 4:
				archetype.Format = Description::Texture::Format::RGBA_8;
				return;
			default:
				FE_CORE_ASSERT(false, "Unimplemented texture format");
			}
		}
	}

	void TextureLoader::UnloadTexture(void* data)
	{
		stbi_image_free(data);
	}

	Description::Texture::Archetype TextureLoader::InspectTexture(const std::filesystem::path& sourceFilePath)
	{
		using namespace Description;
		int width, height, channels;
		int result = 0;
		result = stbi_info(sourceFilePath.string().c_str(), &width, &height, &channels);

		{
			Scratchpad sp;
			std::pmr::string filename("stbi_load - " + sourceFilePath.filename().string<PMR_STRING_TEMPLATE_PARAMS>(), &sp);
			FE_PROFILER_SCOPE(filename.c_str());
			std::pmr::string file_path = sourceFilePath.string<PMR_STRING_TEMPLATE_PARAMS>();
			result = stbi_info(file_path.c_str(), &width, &height, &channels);
		}
		
		Description::Texture::Archetype spec;

		if (!result)
		{
			FE_CORE_ASSERT(false, "Failed to read texture file");
			return spec;
		}

		spec.Type = Description::Texture::Type::Texture2D;
		spec.Format.FromInt(channels); // TO DO: this is a dangerous hack

		return spec;
	}

	bool TextureLoader::IsKnownExtension(const std::pmr::string& extension)
	{
		static const char* s_known_extensions[] = {
			".jpg",
			".jpeg",
			".png",
			".bmp",
			".tga",
			".gif",
			".ppm",
			".pgm"
		};

		for (const auto& known_extension : s_known_extensions)
		{
			if (extension == known_extension)
			{
				return true;
			}
		}

		return false;
	}

	bool TextureLoader::IsKnownAssetType(AssetType assetType)
	{
		static const AssetType s_known_types[] = {
			AssetType::Texture2D
		};

		for (const auto& known_type : s_known_types)
		{
			if (known_type == assetType)
			{
				return true;
			}
		}

		return false;
	}
}