#include "GeometryImport.h"
#include "ImportData.h"
#include "FileHandler.h"

#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>

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
		importData->GeometryData.ImportVariant = ImportVariant_Mesh;
		//importData->GeometryData.Merge = true;
		importData->GeometryData.GLTFTexturePacking = IsGuaranteedStandardTexturePacking(importData->Filepath);
		importData->GeometryData.MaterialPreviewItemSelectedIndex = 0;
	}


	struct ImportRenderMesh
	{
		uint32_t AssimpMeshIndex;
		uint32_t AssimpMaterialIndex;
		uint32_t VertexCount;
		uint32_t IndexCount;
	};

	static void ImportAsModel(const std::filesystem::path& filepath, const ImportData* const importData)
	{
		auto scene = GeometryLoader::InspectSourceFile(importData->Filepath,
			aiPostProcessSteps::aiProcess_PreTransformVertices |
			aiPostProcessSteps::aiProcess_OptimizeMeshes
		);

		FE_CORE_ASSERT(false, "not implemented yet");
		AssetID assetID = NullAssetID;// = AssetManager::CreateAsset();
		AssetHandle<Model> model_handle(assetID);
		auto model_user = model_handle.Use();

		//auto& spec = model_user.GetSpecification();
		//spec.RenderMeshCount = scene->mNumMeshes;

		std::vector<ImportRenderMesh> render_meshes;

		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			auto& render_mesh = render_meshes.emplace_back();

			render_mesh.AssimpMeshIndex = (uint32_t)i;
			render_mesh.AssimpMaterialIndex = scene->mMeshes[i]->mMaterialIndex;
			render_mesh.IndexCount = scene->mMeshes[i]->mNumFaces * 3;
			render_mesh.VertexCount = scene->mMeshes[i]->mNumVertices; 
		}

		//model_user.SetFilepath(importData->Filepath);

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

			//if (importData->GeometryData.ImportMaterials)
			//	emitter << YAML::Key << "Assimp Material Index" << YAML::Value << renderMesh.AssimpMaterialIndex;

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

	static void ImportAsRenderMesh(const std::filesystem::path& filepath, const ImportData* const importData)
	{
	
	}

	static void ImportAsMesh(const std::filesystem::path& filepath, const ImportData* const importData)
	{
		auto scene = GeometryLoader::InspectSourceFile(importData->Filepath,
			aiPostProcessSteps::aiProcess_JoinIdenticalVertices |
			aiPostProcessSteps::aiProcess_OptimizeMeshes |
			aiPostProcessSteps::aiProcess_PreTransformVertices |
			aiPostProcessSteps::aiProcess_RemoveRedundantMaterials |
			aiPostProcessSteps::aiProcess_Triangulate
		);
		
		auto y = Project::GetInstance()->AssetsPath;
		auto z = std::filesystem::current_path();
		auto x = filepath.lexically_relative(z);
		auto w = x.lexically_relative(y);
		const AssetID assetID = AssetManager::AssetCreation::ProjectAsset<Mesh>(w);
		{
			auto mesh_user = AssetUser<Mesh>(assetID);

			AssetManager::SetSourcePath(assetID, importData->Filepath);
			auto& core = mesh_user.GetCoreComponent();
			auto& specification = core.Specification;

			for (size_t i = 0; i < scene->mNumMeshes; i++)
			{
				specification.VertexCount += scene->mMeshes[i]->mNumVertices;
				specification.IndexCount += scene->mMeshes[i]->mNumFaces;
			}
			specification.IndexCount *= 3;
		}
		
		Mesh::SaveMetadata(assetID);
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

	static void RenderMeshList(std::pmr::vector<uint32_t>& meshCountPerMaterial, const aiScene* const scene)
	{
		constexpr ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

		if (ImGui::BeginTable("MeshesTable", 5, flags))
		{
			ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Material", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Vertex Count", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Tri Count", ImGuiTableColumnFlags_NoHide);
			ImGui::TableHeadersRow();

			Scratchpad sp;
			for (size_t i = 0; i < scene->mNumMeshes; i++)
			{
				auto& mesh = scene->mMeshes[i];
				auto matIndex = mesh->mMaterialIndex;
				meshCountPerMaterial[matIndex]++;

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				std::string no = std::to_string(i);
				ImGui::Text(no.c_str());
				ImGui::TableNextColumn();
				ImGui::Text(mesh->mName.C_Str());
				ImGui::TableNextColumn();
				const auto name = scene->mMaterials[matIndex]->GetName();
				std::pmr::string nameLabel(std::to_string(matIndex), &sp);
				nameLabel += ". ";
				nameLabel += name.C_Str();
				ImGui::Text(nameLabel.c_str());
				ImGui::TableNextColumn();
				ImGui::Text("%i", mesh->mNumVertices);
				ImGui::TableNextColumn();
				ImGui::Text("%i", mesh->mNumFaces);
			}

			ImGui::EndTable();
		}
	}

	static void RenderMaterialList(const std::pmr::vector<uint32_t>& meshCountPerMaterial, const aiScene* const scene)
	{
		constexpr ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

		if (ImGui::BeginTable("MaterialsTable", 3, flags))
		{
			ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Mesh Count", ImGuiTableColumnFlags_NoHide);
			ImGui::TableHeadersRow();

			for (size_t i = 0; i < scene->mNumMaterials; i++)
			{
				const auto& mat = scene->mMaterials[i];
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				std::string no = std::to_string(i);
				ImGui::Text(no.c_str());
				ImGui::TableNextColumn();
				const auto name = mat->GetName();
				ImGui::Text(name.C_Str());
				ImGui::TableNextColumn();
				ImGui::Text("%i", meshCountPerMaterial[i]);
			}

			ImGui::EndTable();
		}
	}

	static void RenderMaterialPreview(ImportData* const importData)
	{
		auto& selected_idx = importData->GeometryData.MaterialPreviewItemSelectedIndex;
		auto& scene = importData->GeometryData.Scene;
		
		const auto selected_material_name = scene->mMaterials[selected_idx]->GetName();
		Scratchpad sp;
		std::pmr::string preview(std::to_string(selected_idx), &sp);
		preview += ". " + std::pmr::string(selected_material_name.C_Str(), &sp);

		if (ImGui::BeginCombo("mat prev", preview.c_str()))
		{
			for (size_t n = 0; n < scene->mNumMaterials; n++)
			{
				const bool is_selected = (selected_idx == n);
				auto name = scene->mMaterials[n]->GetName();

				std::pmr::string label(std::to_string(n), &sp);
				label += ". " + std::pmr::string(name.C_Str());

				if (ImGui::Selectable(label.c_str(), is_selected))
					selected_idx = (uint32_t)n;

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

		bool b0 = false; mat->Get(AI_MATKEY_USE_COLOR_MAP, b0); if (!b0) // for some reason this returns the opposite...
		{
			aiString base_color_map; mat->GetTexture(aiTextureType_BASE_COLOR, 0, &base_color_map);
			if (base_color_map.length) ImGui::InputText("Base Color Map", (char*)base_color_map.C_Str(), base_color_map.length + 1);
		}

		if (importData->GeometryData.GLTFTexturePacking)
		{
			aiString omr_map; mat->GetTexture(aiTextureType_UNKNOWN, 0, &omr_map);
			if (omr_map.length) ImGui::InputText("Occlusion-Metalness-Roughness Map", (char*)omr_map.C_Str(), omr_map.length + 1);
		}
		else
		{
			bool b1 = false; mat->Get(AI_MATKEY_USE_METALLIC_MAP, b1); if (b1)
			{
				aiString metalness_map; mat->GetTexture(aiTextureType_METALNESS, 0, &metalness_map);
				if (metalness_map.length) ImGui::InputText("Metalness Map", (char*)metalness_map.C_Str(), metalness_map.length + 1);
				else ImGui::Text("Metalness Map is missing and assimp indicates to use it");
			}

			bool b2 = false; mat->Get(AI_MATKEY_USE_ROUGHNESS_MAP, b2); if (b2)
			{
				aiString roughness_map; mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &roughness_map);
				if (roughness_map.length) ImGui::InputText("Roughness Map", (char*)roughness_map.C_Str(), roughness_map.length + 1);
				else ImGui::Text("Roughness Map is missing and assimp indicates to use it");
			}

			bool b3 = false; mat->Get(AI_MATKEY_USE_AO_MAP, b3); if (b3)
			{
				aiString occlusion_map; mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &occlusion_map);
				if (occlusion_map.length) ImGui::InputText("Occlusion Map", (char*)occlusion_map.C_Str(), occlusion_map.length + 1);
				else ImGui::Text("Occlusion Map is missing and assimp indicates to use it");
			}
		}

		bool b4 = false; mat->Get(AI_MATKEY_USE_EMISSIVE_MAP, b4); if (b4)
		{
			aiString emissive_map; mat->GetTexture(aiTextureType_EMISSION_COLOR, 0, &emissive_map);
			if (emissive_map.length) ImGui::InputText("Emissive Map", (char*)emissive_map.C_Str(), emissive_map.length + 1);
			else ImGui::Text("Emissive Map is missing and assimp indicates to use it");
		}
		
		aiString normal_map_camera; mat->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &normal_map_camera);
		if (normal_map_camera.length) ImGui::InputText("Normal Map Camera", (char*)normal_map_camera.C_Str(), normal_map_camera.length + 1);
		aiString normal_map_tangent; mat->GetTexture(aiTextureType_NORMALS, 0, &normal_map_tangent);
		if (normal_map_tangent.length) ImGui::InputText("Normal Map Tangent", (char*)normal_map_tangent.C_Str(), normal_map_tangent.length + 1);
		
		int unknown_textures_count = mat->GetTextureCount(aiTextureType_UNKNOWN);
		if (unknown_textures_count)
		{
			if (importData->GeometryData.GLTFTexturePacking)
				ImGui::Text("First texture may be Occlusion-Metalness-Roughness Map");
			for (size_t i = 0; i < unknown_textures_count; i++)
			{
				aiString unknown_map; mat->GetTexture(aiTextureType_UNKNOWN, (unsigned int)i, &unknown_map);
				ImGui::Text("%s", unknown_map.C_Str());
			}
		}
	}

	void RenderWindow(ImportData* const importData)
	{
		auto& scene = importData->GeometryData.Scene;

		ImGui::SeparatorText("Geometry Info");
		ImGui::Text("File: %s", importData->Filepath.string().c_str());

		if (ImGui::CollapsingHeader("Nodes", 0))
			RenderHierarchy(scene);

		{
			Scratchpad sp;
			std::pmr::vector<uint32_t> meshCountPerMaterial(scene->mNumMaterials, 0, &sp);

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
		}

		ImGui::SeparatorText("Import Settings");

		ImGui::Text("Import as:");

		int* import_variant = (int*)&importData->GeometryData.ImportVariant;
		
		if (scene->mNumMaterials > 1)
		{
			ImGui::RadioButton("Model", import_variant, ImportVariant_Model);
		}
		else
		{
			ImGui::BeginDisabled();
			ImGui::RadioButton("Model", import_variant, ImportVariant_Model);
			ImGui::EndDisabled();
		}
		ImGui::SameLine();

		if (scene->mNumMaterials == 1)
		{
			ImGui::RadioButton("RenderMesh", import_variant, ImportVariant_RenderMesh);
		}
		else
		{
			ImGui::BeginDisabled();
			ImGui::RadioButton("RenderMesh", import_variant, ImportVariant_RenderMesh);
			ImGui::EndDisabled();
		}
		ImGui::SameLine();
		
		ImGui::RadioButton("Mesh", import_variant, ImportVariant_Mesh);
		
		switch (importData->GeometryData.ImportVariant)
		{
		case ImportVariant_Model:
			break;
		case ImportVariant_RenderMesh:
			break;
		case ImportVariant_Mesh:
			break;
		}

		// setting textures mapping to shader samplers and chanel Swizzle masks
		if (importData->GeometryData.ImportVariant == ImportVariant_Model ||
			importData->GeometryData.ImportVariant == ImportVariant_RenderMesh)
		{
			if (importData->GeometryData.GLTFTexturePacking)
			{

			}
			else
			{

			}
		}
		

		ImGui::Separator();
		if (ImGui::Button("Import As..."))
		{
			Scratchpad sp;
			std::filesystem::path defaultFilepath;
			std::pmr::string filter(&sp);
			
			switch (importData->GeometryData.ImportVariant)
			{
			case ImportVariant_Model:		defaultFilepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<Model		>(importData->Filepath, filter);	break;
			case ImportVariant_RenderMesh:	defaultFilepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<RenderMesh	>(importData->Filepath, filter);	break;
			case ImportVariant_Mesh:		defaultFilepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<Mesh		>(importData->Filepath, filter);	break;
			}

			std::filesystem::path newAssetFilepath = FileDialogs::SaveFile(defaultFilepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str(), filter.c_str());

			if (!newAssetFilepath.empty())
			{
				switch (importData->GeometryData.ImportVariant)
				{
				case ImportVariant_Model:		ImportAsModel(newAssetFilepath, importData); break;
				case ImportVariant_RenderMesh:	ImportAsRenderMesh(newAssetFilepath, importData); break;
				case ImportVariant_Mesh:		ImportAsMesh(newAssetFilepath, importData); break;
				}
				
				importData->Finished = true;
			}
		}
	}
}