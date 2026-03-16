#include "FE_pch.h"
#include "Project.h"

#include "FoolsEngine/Assets/Serialization/YAML.h"

namespace fe
{
	Project* Project::s_Instance = nullptr;

	Project::Project(const std::filesystem::path& filepath)
	{
		FE_PROFILER_FUNC();

		if (s_Instance)
		{
			FE_CORE_ASSERT(false, "One project per app launch");
			delete s_Instance;
		}
		s_Instance = this;

		m_File = filepath.filename();
		m_Directory = filepath.parent_path();
		m_AssetsPath = s_Instance->m_Directory / "Assets";
	}

	Project::~Project()
	{
		s_Instance = nullptr;
	}

	void Project::Create(const std::filesystem::path& filepath)
	{
		FE_PROFILER_FUNC();

		new Project(filepath);

		Serialize();
	}

	void Project::Load(const std::filesystem::path& filepath)
	{
		FE_PROFILER_FUNC();

		new Project(filepath);

		Deserialize();
	}

	void Project::Save()
	{
		Serialize();
	}

	void Project::Serialize()
	{
		auto& inst = *s_Instance;
		YAML::Emitter emitter;
		
		//emitter << YAML::Key << "AssetsPath" << YAML::Value << inst.AssetsPath.string();
		emitter << YAML::Key << "StartScene" << YAML::Value << inst.StartScene;

		emitter << YAML::Key << "BaseAssets" << YAML::Value << YAML::BeginMap;
			emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Default"	<< YAML::Value << inst.BaseAssets.Textures.Default;
				emitter << YAML::Key << "FlatWhite"	<< YAML::Value << inst.BaseAssets.Textures.FlatWhite;
				emitter << YAML::Key << "FlatBlack"	<< YAML::Value << inst.BaseAssets.Textures.FlatBlack;
			emitter << YAML::EndMap;
			emitter << YAML::Key << "Shaders" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Base2DVertex"			<< YAML::Value << inst.BaseAssets.Shaders.Base2DBatchVertex;
				emitter << YAML::Key << "Base2DFragment"		<< YAML::Value << inst.BaseAssets.Shaders.Base2DBatchFragment;
				emitter << YAML::Key << "Base3DVertex"			<< YAML::Value << inst.BaseAssets.Shaders.Base3DVertex;
				emitter << YAML::Key << "Base3DFragmentOpaque"	<< YAML::Value << inst.BaseAssets.Shaders.Base3DFragmentOpaque;
				emitter << YAML::Key << "Base3DFragmentCutout"	<< YAML::Value << inst.BaseAssets.Shaders.Base3DFragmentCutout;
				emitter << YAML::Key << "Base3DFragmentBlend"	<< YAML::Value << inst.BaseAssets.Shaders.Base3DFragmentBlend;
			emitter << YAML::EndMap;
			emitter << YAML::Key << "ShadingModels" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "Base2DFlat"	<< YAML::Value << inst.BaseAssets.ShadingModels.Base2DBatchFlat;
				emitter << YAML::Key << "Base3DOpaque"	<< YAML::Value << inst.BaseAssets.ShadingModels.Base3DOpaque;
				emitter << YAML::Key << "Base3DCutout"	<< YAML::Value << inst.BaseAssets.ShadingModels.Base3DCutout;
				emitter << YAML::Key << "Base3DBlend"	<< YAML::Value << inst.BaseAssets.ShadingModels.Base3DBlend;
			emitter << YAML::EndMap;
			emitter << YAML::Key << "Materials" << YAML::Value << YAML::BeginMap;
				emitter << YAML::Key << "DefaultOpaque"			<< YAML::Value << inst.BaseAssets.Materials.DefaultOpaque;
				emitter << YAML::Key << "DefaultCutout"			<< YAML::Value << inst.BaseAssets.Materials.DefaultCutout;
				emitter << YAML::Key << "DefaultTranslucent"	<< YAML::Value << inst.BaseAssets.Materials.DefaultTranslucent;
			emitter << YAML::EndMap;
		emitter << YAML::EndMap;

		std::ofstream fout(inst.m_Directory / inst.m_File);
		fout << emitter.c_str();
	}

	bool Project::Deserialize()
	{
		FE_PROFILER_FUNC();

		auto& inst = *s_Instance;
		auto path = (inst.m_Directory / inst.m_File).string();
		YAML::Node main_node;
		
		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			main_node = YAML::LoadFile(path);
		}

		//const auto assets_node      = main_node["AssetsPath"];
		const auto start_scene_node = main_node["StartScene"];
		const auto base_assets_node = main_node["BaseAssets"];

		//if (!assets_node) return false;
		if (!start_scene_node) return false;
		if (!base_assets_node) return false;

		//inst.AssetsPath = main_node["AssetsPath"].as<std::string>();
		inst.StartScene = main_node["StartScene"].as<UUID>();

		const auto textures_node       = base_assets_node["Textures"];
		const auto shaders_node        = base_assets_node["Shaders"];
		const auto shading_models_node = base_assets_node["ShadingModels"];
		const auto materials_node      = base_assets_node["Materials"];

		if (!textures_node) return false;
		if (!shaders_node) return false;
		if (!shading_models_node) return false;
		if (!materials_node) return false;

		inst.BaseAssets.Textures.Default	= textures_node["Default"	].as<UUID>();
		inst.BaseAssets.Textures.FlatWhite	= textures_node["FlatWhite"	].as<UUID>();
		inst.BaseAssets.Textures.FlatBlack	= textures_node["FlatBlack"	].as<UUID>();

		inst.BaseAssets.Shaders.Base2DBatchVertex		= shaders_node["Base2DVertex"			].as<UUID>();
		inst.BaseAssets.Shaders.Base2DBatchFragment		= shaders_node["Base2DFragment"		].as<UUID>();
		inst.BaseAssets.Shaders.Base3DVertex			= shaders_node["Base3DVertex"			].as<UUID>();
		inst.BaseAssets.Shaders.Base3DFragmentOpaque	= shaders_node["Base3DFragmentOpaque"	].as<UUID>();
		inst.BaseAssets.Shaders.Base3DFragmentCutout	= shaders_node["Base3DFragmentCutout"	].as<UUID>();
		inst.BaseAssets.Shaders.Base3DFragmentBlend		= shaders_node["Base3DFragmentBlend"	].as<UUID>();

		inst.BaseAssets.ShadingModels.Base2DBatchFlat	= shading_models_node["Base2DFlat"	].as<UUID>();
		inst.BaseAssets.ShadingModels.Base3DOpaque		= shading_models_node["Base3DOpaque"].as<UUID>();
		inst.BaseAssets.ShadingModels.Base3DCutout		= shading_models_node["Base3DCutout"].as<UUID>();
		inst.BaseAssets.ShadingModels.Base3DBlend		= shading_models_node["Base3DBlend"	].as<UUID>();

		inst.BaseAssets.Materials.DefaultOpaque			= materials_node["DefaultOpaque"		].as<UUID>();
		inst.BaseAssets.Materials.DefaultCutout			= materials_node["DefaultCutout"		].as<UUID>();
		inst.BaseAssets.Materials.DefaultTranslucent	= materials_node["DefaultTranslucent"	].as<UUID>();

		FE_LOG_CORE_INFO("Project deserialized");

		return true;
	}
}