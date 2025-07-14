#include <FoolsEngine.h>
#include "CreateAsset.h"

namespace fe
{
	namespace CreateAsset
	{
		bool s_modal_open = false;
		AssetType s_asset_type = AssetType::None;
		std::filesystem::path s_filepath;

		void Init()
		{

		}

		void OpenWindow(const std::filesystem::path& filepath, AssetType assetType)
		{
			s_modal_open = true;
			s_asset_type = assetType;
			s_filepath = filepath;
		}

		void OnImGuiRender()
		{
			if (s_modal_open)
				ImGui::OpenPopup("Asset Creation");
			if (ImGui::BeginPopupModal("Asset Creation", NULL, 0))
			{
				switch (s_asset_type)
				{
				case AssetType::Material:

					break;
				default:
					FE_LOG_CORE_WARN("Creation of {0} asset in editor not implemented", s_asset_type.ToConstCharPtr());
				}

				if (ImGui::Button("Cancel"))
				{
					s_modal_open = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		void CreateMaterial(const std::filesystem::path& filepath)
		{
			Scratchpad sp;

			std::pmr::string filter(&sp);
			filter = std::pmr::string("Material (", &sp) + Material::GetMetaFileExtension() + ")" + std::pmr::string(1, '\0', &sp) + "*" + Material::GetMetaFileExtension() + std::pmr::string(1, '\0', &sp);

			std::filesystem::path defaultFilepath = filepath;
			defaultFilepath = std::filesystem::absolute(defaultFilepath / (std::pmr::string("Material", &sp) + Material::GetMetaFileExtension()));

			std::filesystem::path newAssetFilepath = FileDialogs::SaveFile(defaultFilepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str(), filter.c_str());


			auto assets_folder = std::filesystem::current_path() / Project::GetInstance()->AssetsPath;

			auto assets_path = Project::GetInstance()->AssetsPath;
			auto new_asset_relative_path = newAssetFilepath.lexically_relative(std::filesystem::current_path());
			new_asset_relative_path = new_asset_relative_path.lexically_relative(assets_path);

			auto assetID = AssetManager::AssetCreation::ProjectAsset<Material>(new_asset_relative_path);

			AssetUser<Material>(assetID).MakeMaterial(Renderer::BaseAssets.ShadingModels.Default.Observe());
			Material::SaveMetadata(assetID);
		}
	}
}