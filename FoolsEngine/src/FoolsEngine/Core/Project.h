#pragma once

#include "FoolsEngine\Assets\Asset.h"

#include <filesystem>

namespace fe
{
	class Project
	{
	public:
		static Project* Get() { return s_Instance; }

		static void Create(const std::filesystem::path& filepath);
		static void Load(const std::filesystem::path& filepath);
		static void Save();

		static void Serialize();
		static bool Deserialize();

		struct
		{
			struct {
				UUID Default;
				UUID FlatWhite;
				UUID FlatBlack;
			} Textures;

			struct {
				UUID Base2DVertex;
				UUID Base2DFragment;
				UUID Base3DVertex;
				UUID Base3DFragmentBlend;
				UUID Base3DFragmentCutout;
				UUID Base3DFragmentOpaque;
			} Shaders;

			struct {
				UUID Base2DFlat;
				UUID Base3DOpaque;
				UUID Base3DCutout;
				UUID Base3DBlend;
			} ShadingModels;

			struct {
				UUID Default2DFlat;
				UUID DefaultOpaque;
				UUID DefaultCutout;
				UUID DefaultTranslucent;
			} Materials;
		} BaseAssets;

		std::filesystem::path m_File;
		std::filesystem::path m_Directory;
		std::filesystem::path m_AssetsPath;
		UUID StartScene;

		// tags list? (scene component)

	private:
		Project(const std::filesystem::path& filepath);
		~Project();
		static Project* s_Instance;
	};
}