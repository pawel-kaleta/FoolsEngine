#include "TextureLoader.h"

#include "FoolsEngine/Foundation/Utils/Core.h"
#include "FoolsEngine/Foundation/Context/Pile.h"
#include "FoolsEngine/Foundation/Allocators/Allocator.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Renderer/1 - GAPI/Stream.h"
#include "FoolsEngine/Renderer/5 - Integration/Director.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace fe
{
	void TextureLoader::LoadTexture(const std::filesystem::path& sourceFilePath, const AssetUser& textureUser)
	{
		FE_PROFILER_FUNC();

		Pile p;
		STD_PMR_Allocator pmr(&p);

		std::pmr::string filename("stbi_load - ", &pmr);
		filename += sourceFilePath.filename().string<PMR_STRING_TEMPLATE_PARAMS>(&pmr);
		std::pmr::string file_path = sourceFilePath.string<PMR_STRING_TEMPLATE_PARAMS>(&pmr);


		bool should_flip = Render::GAPI::Platform::FE_PARAMS_GAPI == Render::GAPI::Platform::OpenGL;
		stbi_set_flip_vertically_on_load(should_flip);

		int width, height, channels;
		stbi_uc* data = nullptr;
		{
			FE_PROFILER_SCOPE(filename.c_str());
			data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
		}

		if (!data)
		{
			Context::Log::Core.Error("{0}"_FE, file_path.c_str());
			FE_CORE_ASSERT(false, "Aaaa");
		}

		UInt data_size = width * height * channels;

		Render::GAPI::GID region = Render::GAPI::CreateRegion(Render::Integration::sTextureDownStream, data_size);
		Byte* region_location = Render::GAPI::GetRegionLocation(region);
		std::memcpy(region_location, data, data_size);
		stbi_image_free(data);

		auto& spec = textureUser.Get<Texture2D::Core>().mSpecification;
		spec.Init();
		spec.mType = Render::GAPI::Descriptors::Texture::Type::Texture2D;
		spec.mDimentions = { width, height, 0};
		spec.mMipCount = 1;
		spec.mLayerCount = 1;

		switch (channels)
		{
		case 1: spec.mFormat = Render::GAPI::Descriptors::Texture::Format::R_8; break;
		case 3: spec.mFormat = Render::GAPI::Descriptors::Texture::Format::RGB_8; break;
		case 4: spec.mFormat = Render::GAPI::Descriptors::Texture::Format::RGBA_8; break;
		default:
			FE_CORE_ASSERT(false, "Unimplemented texture format");
		}

		Render::GAPI::CommitRegion(region);


		Render::Integration::sTextureAllocator.CreateTexture(textureUser);

		auto GPU_component = textureUser.GetIfExist<ACTexture2D_GPU<Render::GAPI::Platform::OpenGL>>();

		FE_CORE_ASSERT(GPU_component, "AAAAAA!");

		Render::GAPI::CopyToTextureCmd(GPU_component->mTextureGID, region);

		Render::GAPI::RetireRegionCmd(region);
	}

	Render::GAPI::Descriptors::Texture::Spec TextureLoader::InspectTexture(const std::filesystem::path& sourceFilePath)
	{
		using namespace Render::GAPI::Descriptors;
		int width, height, channels;
		int result = 0;
		result = stbi_info(sourceFilePath.string().c_str(), &width, &height, &channels);

		{
			Pile p;
			STD_PMR_Allocator pmr(&p);
			std::pmr::string filename("stbi_load - ", &pmr);
			filename += sourceFilePath.filename().string<PMR_STRING_TEMPLATE_PARAMS>(&pmr);
			FE_PROFILER_SCOPE(filename.c_str());
			std::pmr::string file_path = sourceFilePath.string<PMR_STRING_TEMPLATE_PARAMS>(&pmr);
			result = stbi_info(file_path.c_str(), &width, &height, &channels);
		}
		
		Render::GAPI::Descriptors::Texture::Spec spec;

		if (!result)
		{
			FE_CORE_ASSERT(false, "Failed to read texture file");
			return spec;
		}

		spec.mType = Render::GAPI::Descriptors::Texture::Type::Texture2D;
		spec.mFormat.FromInt(channels); // TO DO: this is a dangerous hack

		return spec;
	}

	bool TextureLoader::IsKnownExtension(String extension)
	{
		static const String s_known_extensions[] = {
			String(".jpg"_FE),
			String(".jpeg"_FE),
			String(".png"_FE),
			String(".bmp"_FE),
			String(".tga"_FE),
			String(".gif"_FE),
			String(".ppm"_FE),
			String(".pgm"_FE)
		};

		for (const auto& known_extension : s_known_extensions)
		{
			if (CompareStringsEqual(known_extension, extension))
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