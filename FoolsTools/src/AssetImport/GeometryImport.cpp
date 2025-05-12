#include "GeometryImport.h"
#include "ImportData.h"

#include <assimp/pbrmaterial.h>

#include <FoolsEngine.h>

namespace fe::GeometryImport
{
	static bool IsGuaranteedStandardTexturePacking(const std::filesystem::path& path)
	{
		const static std::filesystem::path extensionsGuaranteeing[] = {
			".glb",
			".gltf"
		};
		const std::filesystem::path extension = path.extension();
		return extension == extensionsGuaranteeing[0] || extension == extensionsGuaranteeing[1];
	}

	void InitImport(ImportData* const importData)
	{
		importData->GeometryData.Scene = GeometryLoader::InspectSourceFile(importData->Filepath);

		if (importData->HandleToOverride != nullptr && importData->Type == AssetType::MeshAsset)
			importData->GeometryData.ImportVariant = ImportVariant_Mesh;
		else
			importData->GeometryData.ImportVariant = ImportVariant_Model;
		

		importData->GeometryData.ImportMaterials = true;
		importData->GeometryData.GLTFTexturePacking = IsGuaranteedStandardTexturePacking(importData->Filepath);

		importData->GeometryData.MaterialPreviewItemSelectedIndex = 0;
	}


	static void ImportAsPrefab()
	{
	
	}

	struct ImportRenderMesh
	{
		uint32_t AssimpMeshIndex;
		uint32_t AssimpMaterialIndex;
		uint32_t VertexCount;
		uint32_t IndexCount;
	};

	static void ImportAsModel(const std::filesystem::path& filepath, ImportData* const importData)
	{
		auto scene = GeometryLoader::InspectSourceFile(importData->Filepath,
			aiPostProcessSteps::aiProcess_PreTransformVertices |
			aiPostProcessSteps::aiProcess_OptimizeMeshes
		);

		AssetID assetID = AssetManager::CreateAsset<Model>();
		AssetHandle<Model> model_handle(assetID);
		auto model_user = model_handle.Use();

		auto& spec = model_user.GetSpecification();
		spec.RenderMeshCount = scene->mNumMeshes;

		std::vector<ImportRenderMesh> render_meshes;

		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			auto& render_mesh = render_meshes.emplace_back();

			render_mesh.AssimpMeshIndex = i;
			render_mesh.AssimpMaterialIndex = scene->mMeshes[i]->mMaterialIndex;
			render_mesh.IndexCount = scene->mMeshes[i]->mNumFaces * 3;
			render_mesh.VertexCount = scene->mMeshes[i]->mNumVertices; 
		}

		model_user.SetFilepath(importData->Filepath);

		if (importData->HandleToOverride && importData->Type == AssetType::ModelAsset)
			*(AssetHandle<Model>*)(importData->HandleToOverride) = model_handle;

		YAML::Emitter emitter;

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID"              << YAML::Value << model_handle.GetUUID();
		emitter << YAML::Key << "Source File"       << YAML::Value << importData->Filepath.string();
		emitter << YAML::Key << "Render Mesh Count" << YAML::Value << render_meshes.size();
		emitter << YAML::Key << "Meshes"            << YAML::Value << YAML::BeginSeq;
		for (auto& renderMesh : render_meshes)
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "AssimpMeshIndex" << YAML::Value << renderMesh.AssimpMeshIndex;

			if (importData->GeometryData.ImportMaterials)
				emitter << YAML::Key << "Assimp Material Index" << YAML::Value << renderMesh.AssimpMaterialIndex;

			emitter << YAML::Key << "Index Count"  << YAML::Value << renderMesh.IndexCount;
			emitter << YAML::Key << "Vertex Count" << YAML::Value << renderMesh.VertexCount;

			emitter << YAML::EndMap;
		}
		emitter << YAML::EndSeq;

		emitter << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << emitter.c_str();
		fout.close();
	}

	static void ImportAsMesh(const std::filesystem::path& filepath, ImportData* const importData)
	{
		if (importData->GeometryData.ImportMaterials)
		{
			auto material_to_import_index = importData->GeometryData.MaterialPreviewItemSelectedIndex;
			//TO DO
		}

		auto scene = GeometryLoader::InspectSourceFile(importData->Filepath,
			aiPostProcessSteps::aiProcess_PreTransformVertices |
			aiPostProcessSteps::aiProcess_OptimizeMeshes
		);
		
		AssetID assetID = AssetManager::CreateAsset<Mesh>();
		AssetHandle<Mesh> mesh_handle(assetID);
		auto mesh_user = mesh_handle.Use();

		mesh_user.SetFilepath(importData->Filepath);

		auto& specification = mesh_user.GetSpecification();
		specification.Submeshes = scene->mNumMeshes;
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			specification.VertexCount  += scene->mMeshes[i]->mNumVertices;
			specification.IndicesCount += scene->mMeshes[i]->mNumFaces;
		}
		specification.IndicesCount *= 3;

		if (importData->HandleToOverride && importData->Type == AssetType::MeshAsset)
			*(AssetHandle<Mesh>*)(importData->HandleToOverride) = mesh_handle;

		YAML::Emitter emitter;

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID"         << YAML::Value << mesh_handle.GetUUID();
		emitter << YAML::Key << "Source File"  << YAML::Value << importData->Filepath.string();
		emitter << YAML::Key << "Vertex Count" << YAML::Value << specification.VertexCount;
		emitter << YAML::Key << "Index Count"  << YAML::Value << specification.IndicesCount;
		emitter << YAML::Key << "Submeshes"    << YAML::Value << specification.Submeshes;
		emitter << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << emitter.c_str();
		fout.close();
	}


	static void DisplayNode(const aiScene* const scene, const aiNode* const node)
	{
		constexpr ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAllColumns;

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if (node->mNumChildren > 0)
		{
			bool open = ImGui::TreeNodeEx(node->mName.C_Str(), tree_node_flags);
			ImGui::TableNextColumn();
			if (node->mTransformation.IsIdentity())
				ImGui::Text("NO");
			else
				ImGui::Text("YES");
			ImGui::TableNextColumn();
			ImGui::Text("%i", node->mNumMeshes);

			if (open)
			{
				for (size_t i = 0; i < node->mNumChildren; i++)
					DisplayNode(scene, node->mChildren[i]);
				ImGui::TreePop();
			}

		}
		else
		{
			ImGui::TreeNodeEx(node->mName.C_Str(), tree_node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen);
			ImGui::TableNextColumn();
			if (node->mTransformation.IsIdentity())
				ImGui::Text("NO");
			else
				ImGui::Text("YES");
			ImGui::TableNextColumn();
			ImGui::Text("%i", node->mNumMeshes);
		}
	}

	static void RenderHierarchy(const aiScene* const scene)
	{
		constexpr ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

		if (ImGui::BeginTable("NodesTable", 4, flags))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Transform", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Mesh Count", ImGuiTableColumnFlags_NoHide);
			ImGui::TableHeadersRow();

			DisplayNode(scene, scene->mRootNode);

			ImGui::EndTable();
		}
	}

	static void RenderMeshList(std::vector<uint32_t>& meshCountPerMaterial, const aiScene* const scene)
	{
		constexpr ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

		if (ImGui::BeginTable("MeshesTable", 4, flags))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Material", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Vertex Count", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Tri Count", ImGuiTableColumnFlags_NoHide);
			ImGui::TableHeadersRow();

			for (size_t i = 0; i < scene->mNumMeshes; i++)
			{
				auto& mesh = scene->mMeshes[i];
				auto matIndex = mesh->mMaterialIndex;
				meshCountPerMaterial[matIndex]++;

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text(mesh->mName.C_Str());
				ImGui::TableNextColumn();
				const auto name = scene->mMaterials[matIndex]->GetName();
				const std::string nameLabel = std::to_string(i) + ". " + std::string(name.C_Str());
				ImGui::Text(nameLabel.c_str());
				ImGui::TableNextColumn();
				ImGui::Text("%i", mesh->mNumVertices);
				ImGui::TableNextColumn();
				ImGui::Text("%i", mesh->mNumFaces);
			}

			ImGui::EndTable();
		}
	}

	static void RenderMaterialList(const std::vector<uint32_t>& meshCountPerMaterial, const aiScene* const scene)
	{
		constexpr ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

		//if (ImGui::BeginTable("MaterialsTable", 3, flags))
		if (ImGui::BeginTable("MaterialsTable", 2, flags))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Mesh Count", ImGuiTableColumnFlags_NoHide);
			//ImGui::TableSetupColumn("bbbbb", ImGuiTableColumnFlags_NoHide);
			ImGui::TableHeadersRow();

			for (size_t i = 0; i < scene->mNumMaterials; i++)
			{
				const auto& mat = scene->mMaterials[i];
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				const auto name = mat->GetName();
				const std::string nameLabel = std::to_string(i) + ". " + std::string(name.C_Str());
				ImGui::Text(nameLabel.c_str());
				ImGui::TableNextColumn();
				ImGui::Text("%i", meshCountPerMaterial[i]);
				//ImGui::TableNextColumn();
				//ImGui::Text("NO");
			}

			ImGui::EndTable();
		}
	}

	static void RenderMaterialPreview(ImportData* const importData)
	{
		auto& selected_idx = importData->GeometryData.MaterialPreviewItemSelectedIndex;
		auto& scene = importData->GeometryData.Scene;
		// Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
		const auto selected_material_name = scene->mMaterials[selected_idx]->GetName();
		std::string preview = std::to_string(selected_idx) + ". " + std::string(selected_material_name.C_Str());

		if (ImGui::BeginCombo("mat prev", preview.c_str()))
		{
			for (size_t n = 0; n < scene->mNumMaterials; n++)
			{
				const bool is_selected = (selected_idx == n);
				auto name = scene->mMaterials[n]->GetName();

				std::string label = std::to_string(n) + ". " + std::string(name.C_Str());
				if (ImGui::Selectable(label.c_str(), is_selected))
					selected_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		auto& mat = scene->mMaterials[selected_idx];

		ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoPicker;
		
		aiColor3D base_color;     mat->Get(AI_MATKEY_BASE_COLOR,        base_color); if (!base_color.IsBlack())                       ImGui::ColorEdit3("Base Color", (float*)&base_color, flags);
		aiColor3D diffuse;        mat->Get(AI_MATKEY_COLOR_DIFFUSE,     diffuse   ); if (diffuse != base_color && !diffuse.IsBlack()) ImGui::ColorEdit3("Diffuse",    (float*)&diffuse,    flags);
		aiColor3D specular;       mat->Get(AI_MATKEY_COLOR_SPECULAR,    specular  ); if (!specular.IsBlack())                         ImGui::ColorEdit3("Specular",   (float*)&specular,   flags);
		aiColor3D ambient;        mat->Get(AI_MATKEY_COLOR_AMBIENT,     ambient   ); if (!ambient.IsBlack())                          ImGui::ColorEdit3("Ambient",    (float*)&ambient,    flags);
		aiColor3D emissive;       mat->Get(AI_MATKEY_COLOR_EMISSIVE,    emissive  ); if (!emissive.IsBlack())                         ImGui::ColorEdit3("Emissive",   (float*)&emissive,   flags);
		float     metalness = 0;  mat->Get(AI_MATKEY_METALLIC_FACTOR,   metalness ); if (metalness)                                   ImGui::InputFloat("Metalness",          &metalness);
		float     roughness = 0;  mat->Get(AI_MATKEY_ROUGHNESS_FACTOR,  roughness ); if (roughness)                                   ImGui::InputFloat("Roughness",          &roughness);
		float     anisotropy = 0; mat->Get(AI_MATKEY_ANISOTROPY_FACTOR, anisotropy); if (anisotropy)                                  ImGui::InputFloat("Anisotropy",         &anisotropy);

		bool b0; mat->Get(AI_MATKEY_USE_COLOR_MAP, b0); if (b0)
		{
			aiString base_color_map; mat->GetTexture(aiTextureType_BASE_COLOR, 0, &base_color_map);
			ImGui::InputText("Base Color Map", (char*)base_color_map.C_Str(), base_color_map.length + 1);

			aiString diffuse_map; mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_map);
			if (base_color_map != diffuse_map) ImGui::InputText("Diffuse Map", (char*)diffuse_map.C_Str(), diffuse_map.length + 1);
		}

		if (importData->GeometryData.GLTFTexturePacking)
		{
			aiString omr_map; mat->GetTexture(aiTextureType_UNKNOWN, 0, &omr_map);
			ImGui::InputText("Occlusion-Metalness-Roughness Map", (char*)omr_map.C_Str(), omr_map.length + 1);
		}
		else
		{
			bool b1; mat->Get(AI_MATKEY_USE_METALLIC_MAP, b1); if (b1)
			{
				aiString metalness_map; mat->GetTexture(aiTextureType_DIFFUSE, 0, &metalness_map);
				if (metalness_map.length) ImGui::InputText("Metalness Map", (char*)metalness_map.C_Str(), metalness_map.length + 1);
			}

			bool b2; mat->Get(AI_MATKEY_USE_ROUGHNESS_MAP, b2); if (b2)
			{
				aiString roughness_map; mat->GetTexture(aiTextureType_DIFFUSE, 0, &roughness_map);
				if (roughness_map.length) ImGui::InputText("Roughness Map", (char*)roughness_map.C_Str(), roughness_map.length + 1);
			}

			bool b3; mat->Get(AI_MATKEY_USE_AO_MAP, b3); if (b3)
			{
				aiString occlusion_map; mat->GetTexture(aiTextureType_DIFFUSE, 0, &occlusion_map);
				if (occlusion_map.length) ImGui::InputText("Occlusion Map", (char*)occlusion_map.C_Str(), occlusion_map.length + 1);
			}
		}

		bool b4; mat->Get(AI_MATKEY_USE_EMISSIVE_MAP, b4); if (b4)
		{
			aiString emissive_map; mat->GetTexture(aiTextureType_EMISSION_COLOR, 0, &emissive_map);
			if (emissive_map.length)
				ImGui::InputText("Emissive Map", (char*)emissive_map.C_Str(), emissive_map.length + 1);
		}
		
		aiString normal_map_a; mat->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &normal_map_a);
		aiString normal_map_b; mat->GetTexture(aiTextureType_NORMALS, 0, &normal_map_b);
		if (normal_map_a.length) ImGui::InputText("Normal Map", (char*)normal_map_a.C_Str(), normal_map_a.length + 1);
		if (normal_map_b.length && normal_map_a != normal_map_b) ImGui::InputText("Normal Map 2", (char*)normal_map_b.C_Str(), normal_map_b.length + 1);

		int unknown_textures_count = mat->GetTextureCount(aiTextureType_UNKNOWN);
		for (size_t i = importData->GeometryData.GLTFTexturePacking; i < unknown_textures_count; i++) // 0 is omr
		{
			aiString unknown_map; mat->GetTexture(aiTextureType_UNKNOWN, i, &unknown_map);
			ImGui::Text("%s", unknown_map.C_Str());
		}
	}

	void RenderWindow(ImportData* const importData)
	{
		auto& scene = importData->GeometryData.Scene;

		ImGui::SeparatorText("Geometry Info");
		ImGui::Text("File: %s", importData->Filepath.string().c_str());

		if (ImGui::CollapsingHeader("Nodes", 0))
			RenderHierarchy(scene);

		std::vector<uint32_t> meshCountPerMaterial(scene->mNumMaterials, 0);

		if (ImGui::CollapsingHeader("Meshes", 0))
			RenderMeshList(meshCountPerMaterial, scene);
		else
		{
			for (size_t i = 0; i < scene->mNumMeshes; i++)
			{
				auto matIndex = scene->mMeshes[i]->mMaterialIndex;
				meshCountPerMaterial[matIndex]++;
			}
		}

		if (ImGui::CollapsingHeader("Materials"))
			RenderMaterialList(meshCountPerMaterial, scene);

		if (scene->mNumMaterials)
		{
			if (ImGui::CollapsingHeader("Material Preview"))
				RenderMaterialPreview(importData);
		}

		ImGui::SeparatorText("Import Settings");

		ImGui::Text("Import as:");
		int* import_variant = (int*)&importData->GeometryData.ImportVariant;

		ImGui::RadioButton("Prefab", import_variant, ImportVariant_Prefab);
		ImGui::SameLine();
		ImGui::RadioButton("Model", import_variant, ImportVariant_Model);
		ImGui::SameLine();
		ImGui::RadioButton("Mesh", import_variant, ImportVariant_Mesh);

		ImGui::Checkbox("Import materials", &importData->GeometryData.ImportMaterials);
		if (importData->GeometryData.ImportMaterials)
		{
			if (*import_variant == ImportVariant_Mesh)
			{
				auto& selected_idx = importData->GeometryData.MaterialPreviewItemSelectedIndex;
				auto& scene = importData->GeometryData.Scene;
				// Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
				const auto selected_material_name = scene->mMaterials[selected_idx]->GetName();
				std::string preview = std::to_string(selected_idx) + ". " + std::string(selected_material_name.C_Str());

				if (ImGui::BeginCombo("mat select", preview.c_str()))
				{
					for (size_t n = 0; n < scene->mNumMaterials; n++)
					{
						const bool is_selected = (selected_idx == n);
						auto name = scene->mMaterials[n]->GetName();

						std::string label = std::to_string(n) + ". " + std::string(name.C_Str());
						if (ImGui::Selectable(label.c_str(), is_selected))
							selected_idx = n;

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}

			if (importData->GeometryData.GLTFTexturePacking)
			{

			}
			else
			{

				// setting textures mapping to shader samplers and chanel Swizzle masks
			}
		}

		ImGui::Separator();
		if (ImGui::Button("Import As..."))
		{
			std::string alias;
			std::string extension;
			switch (importData->GeometryData.ImportVariant)
			{
			case ImportVariant_Prefab:
				alias = "";
				extension = "";
				break;
			case ImportVariant_Model:
				//alias = Model::GetProxyExtensionAlias();
				//extension = Model::GetProxyExtension();
				break;
			case ImportVariant_Mesh:
				//alias = Mesh::GetProxyExtensionAlias();
				//extension = Mesh::GetProxyExtension();
				break;
			}

			std::string filter = alias + " (" + extension + ")" + std::string(1, '\0') + "*" + extension + std::string(1, '\0');
			std::filesystem::path defaultFilepath = importData->Filepath;

			defaultFilepath.replace_extension(std::filesystem::path(extension));

			std::filesystem::path newAssetFilepath = FileDialogs::SaveFile(defaultFilepath.string().c_str(), filter.c_str());

			if (!newAssetFilepath.empty())
			{
				switch (importData->GeometryData.ImportVariant)
				{
				case ImportVariant_Prefab: ImportAsPrefab(); break;
				case ImportVariant_Model:  ImportAsModel(newAssetFilepath, importData); break;
				case ImportVariant_Mesh:   ImportAsMesh(newAssetFilepath, importData); break;
				}

				importData->Finished = true;
			}
		}
	}
}