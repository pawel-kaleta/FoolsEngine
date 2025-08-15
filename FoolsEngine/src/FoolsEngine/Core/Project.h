#pragma once

#include "FoolsEngine\Assets\Asset.h"

#include <filesystem>

namespace fe
{
	class Project
	{
	public:
		static Project* GetInstance() { return s_Instance; }

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
				UUID Base2D;
				UUID Base3D;
			} Shaders;

			struct {
				UUID Default;
			} ShadingModels;

			struct {
				UUID Default;
			} Materials;
		} BaseAssets;

		std::filesystem::path File;
		std::filesystem::path Directory;
		std::filesystem::path AssetsPath;
		UUID StartScene;

		// tags list? (scene component)

	private:
		Project(const std::filesystem::path& filepath);
		~Project();
		static Project* s_Instance;
	};
}