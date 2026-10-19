#include <FoolsEngine.h>
#include "CreateAsset.h"

namespace fe
{
	namespace CreateAsset
	{
		void CreateMaterial(const std::filesystem::path& filepath)
		{
			Pile p;
			STD_PMR_Allocator pmr(&p);

			std::pmr::string filter(&pmr);
			filter = std::pmr::string("Material (", &pmr) + Material::GetMetaFileExtension() + ")" + std::pmr::string(1, '\0', &pmr) + "*" + Material::GetMetaFileExtension() + std::pmr::string(1, '\0', &pmr);

			std::filesystem::path default_filepath = filepath;
			default_filepath = std::filesystem::absolute(default_filepath / (std::pmr::string("Material", &pmr) + Material::GetMetaFileExtension()));

			std::filesystem::path new_asset_filepath = FileDialogs::SaveFile(default_filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&pmr).c_str(), filter.c_str());

			auto assets_folder = std::filesystem::absolute(std::filesystem::current_path() / Project::Get()->m_AssetsPath);

			auto new_asset_relative_path = new_asset_filepath.lexically_relative(assets_folder);

			auto assetID = AssetManager::AssetCreation::ProjectAsset<Material>(new_asset_relative_path);

			FE_CORE_ASSERT(false, "Not implemented");
			//AssetUser<Material>(assetID).MakeMaterial(Renderer::BaseAssets.ShadingModels.Base3DOpaque.Observe());
			YAML::Emitter emitter;
			//Material::SaveMetadata(emitter, assetID);
			std::ofstream fout(Project::Get()->m_AssetsPath / AssetObserver(assetID).GetFilepath());
			fout << emitter.c_str();
		}
	}
}