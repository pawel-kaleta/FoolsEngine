#include <FoolsEngine.h>
#include "CreateAsset.h"

namespace fe
{
	namespace CreateAsset
	{
		void CreateMaterial(const std::filesystem::path& filepath)
		{
			Scratchpad sp;

			std::pmr::string filter(&sp);
			filter = std::pmr::string("Material (", &sp) + Material::GetMetaFileExtension() + ")" + std::pmr::string(1, '\0', &sp) + "*" + Material::GetMetaFileExtension() + std::pmr::string(1, '\0', &sp);

			std::filesystem::path defaultFilepath = filepath;
			defaultFilepath = std::filesystem::absolute(defaultFilepath / (std::pmr::string("Material", &sp) + Material::GetMetaFileExtension()));

			std::filesystem::path newAssetFilepath = FileDialogs::SaveFile(defaultFilepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str(), filter.c_str());

			auto assets_folder = std::filesystem::absolute(std::filesystem::current_path() / Project::GetInstance()->AssetsPath);

			auto new_asset_relative_path = newAssetFilepath.lexically_relative(assets_folder);

			auto assetID = AssetManager::AssetCreation::ProjectAsset<Material>(new_asset_relative_path);

			AssetUser<Material>(assetID).MakeMaterial(Renderer::BaseAssets.ShadingModels.Default.Observe());
			Material::SaveMetadata(assetID);
		}
	}
}