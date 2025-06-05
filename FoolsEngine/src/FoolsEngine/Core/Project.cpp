#include "FE_pch.h"
#include "Project.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

namespace fe
{
	Project* Project::s_Instance;

	void Project::Load(const std::filesystem::path& filepath)
	{
		s_Instance->File = filepath.filename();
		s_Instance->Directory = filepath.parent_path();

		Deserialize();
	}

	void Project::Serialize()
	{
		auto& inst = *s_Instance;
		YAML::Emitter emitter;
		
		emitter << YAML::Key << "AssetsPath" << YAML::Value << inst.AssetsPath.string();
		emitter << YAML::Key << "StartScene" << YAML::Value << inst.StartScene;

		emitter << YAML::Key << "BaseAssets" << YAML::Value << YAML::BeginMap;
			emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Default" << YAML::Value << inst.BaseAssets.Textures.Default;
				emitter << YAML::Key << "FlatWhite" << YAML::Value << inst.BaseAssets.Textures.FlatWhite;
			emitter << YAML::EndMap;
			emitter << YAML::Key << "Shaders" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Base2D" << YAML::Value << inst.BaseAssets.Shaders.Base2D;
				emitter << YAML::Key << "Base3D" << YAML::Value << inst.BaseAssets.Shaders.Base3D;
			emitter << YAML::EndMap;
			emitter << YAML::Key << "ShadingModels" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Default" << YAML::Value << inst.BaseAssets.ShadingModels.Default;
			emitter << YAML::EndMap;
			emitter << YAML::Key << "Materials" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Default" << YAML::Value << inst.BaseAssets.Materials.Default;
			emitter << YAML::EndMap;
		emitter << YAML::EndMap;


		std::ofstream fout(inst.Directory / inst.File);
		fout << emitter.c_str();
	}

	bool Project::Deserialize()
	{
		auto& inst = *s_Instance;
		auto path = (inst.Directory / inst.File).string();
		YAML::Node main_node = YAML::LoadFile(path);

		const auto assets_node      = main_node["AssetsPath"];
		const auto start_scene_node = main_node["StartScene"];
		const auto base_assets_node = main_node["BaseAssets"];

		if (!assets_node) return false;
		if (!start_scene_node) return false;
		if (!base_assets_node) return false;

		inst.AssetsPath = main_node["AssetsPath"].as<std::string>();
		inst.StartScene = main_node["StartScene"].as<UUID>();

		const auto textures_node       = base_assets_node["Textures"];
		const auto shaders_node        = base_assets_node["Shaders"];
		const auto shading_models_node = base_assets_node["ShadingModels"];
		const auto materials_node      = base_assets_node["Materials"];

		if (!textures_node) return false;
		if (!shaders_node) return false;
		if (!shading_models_node) return false;
		if (!materials_node) return false;

		inst.BaseAssets.Textures.Default = textures_node["Default"].as<UUID>();
		inst.BaseAssets.Textures.FlatWhite = textures_node["FlatWhite"].as<UUID>();
		inst.BaseAssets.Shaders.Base2D = shaders_node["Base2D"].as<UUID>();
		inst.BaseAssets.Shaders.Base3D = shaders_node["Base3D"].as<UUID>();
		inst.BaseAssets.ShadingModels.Default = shading_models_node["Default"].as<UUID>();
		inst.BaseAssets.Materials.Default = materials_node["Default"].as<UUID>();
	}
}