#pragma once

#include "FoolsEngine\Assets\Asset.h"

#include <filesystem>

namespace fe
{
	class Project
	{
	public:
		static const Project& GetInstance() { return *s_Instance; }
		
		static void Load(const std::filesystem::path& filepath);

		static void Serialize();
		static bool Deserialize();

		struct
		{
			struct {
				UUID Default;
				UUID FlatWhite;
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
		friend class Application;
		Project() { s_Instance = this; }
		void Startup() {};
		void Shutdown() {};
		static Project* s_Instance;
	};
}