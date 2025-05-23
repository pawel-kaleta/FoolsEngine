#include "FE_pch.h"
#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace fe
{
	void TextureLoader::LoadTexture(const std::filesystem::path& sourceFilePath, AssetUser<Texture2D>& textureUser)
	{
		auto& acDataLocation = textureUser.GetDataComponent().Data;
		if (acDataLocation)
			return;

		//TO DO: dont override specification, use import settings
		auto& spec = textureUser.GetDataComponent().Specification;
		int width, height, channels;

		// TO DO: flipping should be happennig when uploding to gpu, not when loading from disk
		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = stbi_load(sourceFilePath.string().c_str(), &width, &height, &channels, 0);

		acDataLocation = data;

		FE_LOG_CORE_DEBUG("Loading texture, AssetID: {0}, Channels: {1}", textureUser.GetID(), channels);
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

		if (!result)
		{
			FE_CORE_ASSERT(false, "Failed to read texture file");
			return Specification();
		}

		Specification spec;
		spec.Components = (Components)channels;
		spec.Format = (Format)channels;
		spec.Width = width;
		spec.Height = height;

		return spec;
	}

	bool TextureLoader::IsKnownExtension(const std::pmr::string& extension)
	{
		static std::pmr::string knownExtensions[] = {
			".jpg",
			".jpeg",
			".png",
			".bmp",
			".tga",
			".gif",
			".ppm",
			".pgm"
		};

		for (auto& knownExtension : knownExtensions)
		{
			if (extension == knownExtension)
			{
				return true;
			}
		}

		return false;
	}
}