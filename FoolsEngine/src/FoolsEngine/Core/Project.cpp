#include "FE_pch.h"
#include "Project.h"

#include "FoolsEngine\Assets\Serializers\YAML.h"

namespace fe
{
	Project* Project::s_Instance;

	void Project::Load(const std::filesystem::path& filepath)
	{
		s_Instance->File = filepath.filename();
		s_Instance->Directory = filepath.parent_path();
	}

	void Project::Serialize()
	{
		auto& Inst = *s_Instance;
		YAML::Emitter emitter;

		emitter << YAML::BeginMap;
		
		emitter << YAML::Key << "AssetsPath" << YAML::Value << Inst.AssetsPath.string();
		emitter << YAML::Key << "StartScene" << YAML::Value << Inst.StartScene.string();

		emitter << YAML::Key << "BaseAssets" << YAML::Value << YAML::BeginMap;
			emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Default" << YAML::Value << Inst.BaseAssets.Textures.Default;
				emitter << YAML::Key << "FlatWhite" << YAML::Value << Inst.BaseAssets.Textures.FlatWhite;
			emitter << YAML::EndMap;
			emitter << YAML::Key << "Shaders" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Base2D" << YAML::Value << Inst.BaseAssets.Shaders.Base2D;
				emitter << YAML::Key << "Base3D" << YAML::Value << Inst.BaseAssets.Shaders.Base3D;
			emitter << YAML::EndMap;
			emitter << YAML::Key << "ShadingModels" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Default" << YAML::Value << Inst.BaseAssets.ShadingModels.Default;
			emitter << YAML::EndMap;
			emitter << YAML::Key << "Materials" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Default" << YAML::Value << Inst.BaseAssets.Materials.Default;
			emitter << YAML::EndMap;
		emitter << YAML::EndMap;

		emitter << YAML::EndMap;

		std::ofstream fout(Inst.Directory / Inst.File);
		fout << emitter.c_str();
	}
}