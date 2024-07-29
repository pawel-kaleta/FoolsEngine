#include "FE_pch.h"

#include "TextureLoader.h"
#include "FoolsEngine\Debug\Asserts.h"

namespace fe
{
	void TextureLoader::LoadTexture(const std::filesystem::path& filePath, AssetUser<Texture2D>& textureUser)
	{
		auto& acDataLocation = textureUser.GetDataLocation();
		if (acDataLocation.Data)
			return;

		textureUser.GetFilepath().Filepath = filePath;
		auto& spec = textureUser.GetOrEmplaceSpecification().Specification;
		int width, height, channels;

		// TO DO: flipping should be happennig when uploding to gpu, not when loading from disk
		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = stbi_load(filePath.generic_string().c_str(), &width, &height, &channels, 0);

		acDataLocation.Data = data;

		FE_LOG_CORE_DEBUG("Loading texture, AssetID: {0}, Channels: {1}", textureUser.GetID(), channels);
		FE_CORE_ASSERT(data, "Failed to load image!");
		spec.Width = width;
		spec.Height = height;
		spec.Type = TextureData::Type::Texture2D;

		switch (channels)
		{
		case 1:
			spec.Components = TextureData::Components::R_F;
			spec.Format = TextureData::Format::R_FLOAT_32;
			return;
		case 3:
			spec.Components = TextureData::Components::RGB_F;
			spec.Format = TextureData::Format::RGB_FLOAT_8;
			return;
		case 4:
			spec.Components = TextureData::Components::RGBA_F;
			spec.Format = TextureData::Format::RGBA_FLOAT_8;
			return;
		default:
			FE_CORE_ASSERT(false, "Unimplemented texture format");
		}
	}

	void TextureLoader::UnloadTexture(AssetUser<Texture2D>& textureUser)
	{
		auto& dataPtr = textureUser.GetDataLocation().Data;
		stbi_image_free(dataPtr);
		dataPtr = nullptr;
	}
}