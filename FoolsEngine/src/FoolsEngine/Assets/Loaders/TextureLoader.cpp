#include "FE_pch.h"
#include "TextureLoader.h"

#include "FoolsEngine\Core\Project.h"
#include "FoolsEngine\Memory\Scratchpad.h"

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
			FE_PROFILER_SCOPE("Specification Init");

			data_location = data;

			FE_CORE_ASSERT(data, "Failed to load image!");
			spec.Width = width;
			spec.Height = height;

			switch (channels)
			{
			case 1:
				spec.Components = TextureData::Components::R;
				spec.Format = TextureData::Format::R_8;
				return;
			case 3:
				spec.Components = TextureData::Components::RGB;
				spec.Format = TextureData::Format::RGB_8;
				return;
			case 4:
				spec.Components = TextureData::Components::RGBA;
				spec.Format = TextureData::Format::RGBA_8;
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

	TextureData::Specification TextureLoader::InspectTexture(const std::filesystem::path& sourceFilePath)
	{
		using namespace TextureData;
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

		if (!result)
		{
			FE_CORE_ASSERT(false, "Failed to read texture file");
			return Specification();
		}

		Specification spec;
		spec.Width = width;
		spec.Height = height;
		spec.Components.FromInt(channels);
		spec.Format.FromInt(channels);

		return spec;
	}

	bool TextureLoader::IsKnownExtension(const std::pmr::string& extension)
	{
		static const char* knownExtensions[] = {
			".jpg",
			".jpeg",
			".png",
			".bmp",
			".tga",
			".gif",
			".ppm",
			".pgm"
		};

		for (const auto& knownExtension : knownExtensions)
		{
			if (extension == knownExtension)
			{
				return true;
			}
		}

		return false;
	}

	bool TextureLoader::IsKnownAssetType(AssetType assetType)
	{
		static const AssetType knownTypes[] = {
			AssetType::Texture2D
		};

		for (const auto& knownType : knownTypes)
		{
			if (knownType == assetType)
			{
				return true;
			}
		}

		return false;
	}
}