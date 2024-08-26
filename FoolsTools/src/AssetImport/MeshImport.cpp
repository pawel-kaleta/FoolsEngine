#include "MeshImport.h"

#include <FoolsEngine.h>

#include <filesystem>

namespace fe
{
	Scope<const aiScene> s_Scene;
	std::filesystem::path MeshImport::s_Filepath;
	ACMeshSpecification s_Specification;
	AssetHandle<Mesh>* s_Handle;
	static char str0[128];

	void MeshImport::RenderWindow()
	{
		bool dummy_true = true;
		ImGui::BeginDisabled();
		{
			ImGui::Text("Not implemented yet");

			if (s_Scene->HasAnimations())
				ImGui::Checkbox("Animations", &dummy_true);

			if (s_Scene->HasCameras())
				ImGui::Checkbox("Camera", &dummy_true);

			if (s_Scene->HasLights())
				ImGui::Checkbox("Lights", &dummy_true);

			if (s_Scene->HasMaterials())
				ImGui::Checkbox("Materials", &dummy_true);

			if (s_Scene->hasSkeletons())
				ImGui::Checkbox("Skeletons", &dummy_true);

			if (s_Scene->HasTextures())
				ImGui::Checkbox("Textures", &dummy_true);
		}
		ImGui::EndDisabled();
		ImGui::Separator();

		if (s_Scene->HasMeshes())
			ImGui::Checkbox("Meshes", &dummy_true);

	}

	void MeshImport::InitImport(const std::filesystem::path& filepath, AssetHandleBase* optionalBaseHandle)
	{
		s_Handle = (AssetHandle<Mesh>*)optionalBaseHandle;
		s_Filepath = filepath;
		s_Scene = MeshLoader::InspectSourceFile(filepath);
		s_Specification = ACMeshSpecification();

		if (s_Scene->HasMaterials())
		{
			s_Specification.AssimpMaterialIndex = 0;
		}

		memset(str0, 0, sizeof(str0));
		strncpy_s(str0, "Mesh name", sizeof(str0));
	}

	void MeshImport::ImportAs(const std::filesystem::path& filepath)
	{
		if (filepath.empty())
			return;

		for (size_t i = 0; i < s_Scene->mNumMeshes; i++)
		{
			if (s_Scene->mMeshes[i]->mMaterialIndex != s_Specification.AssimpMaterialIndex)
				continue;

			s_Specification.VertexCount += s_Scene->mMeshes[i]->mNumVertices;
			s_Specification.IndicesCount += s_Scene->mMeshes[i]->mNumFaces * s_Scene->mMeshes[i]->mFaces[0].mNumIndices;
			s_Specification.AssimpMeshesIndexes.emplace_back(i);
		}

		YAML::Emitter emitter;

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << UUID();
		emitter << YAML::Key << "Source File"  << YAML::Value << s_Filepath.string();
		emitter << YAML::Key << "Vertex Count" << YAML::Value << s_Specification.VertexCount;
		emitter << YAML::Key << "Index Count"  << YAML::Value << s_Specification.IndicesCount;
		emitter << YAML::Key << "Assimp Material Index" << YAML::Value << s_Specification.AssimpMaterialIndex;
		emitter << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << emitter.c_str();
		fout.close();

		AssetID assetID = AssetManager::CreateAsset<Mesh>(filepath);
		AssetHandle<Mesh> meshHandle(assetID);

		if (s_Handle)
			*s_Handle = meshHandle;
		s_Handle = nullptr;

		meshHandle.Use().GetOrEmplaceSpecification() = s_Specification;
	}
}