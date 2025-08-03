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
		auto scene = GeometryLoader::InspectSourceFile(importData->Filepath);
		auto texture_packing = IsGuaranteedStandardTexturePacking(importData->Filepath);
		
		auto& data = importData->GeometryData;
		auto& set_textures = data.Materials.SetTextures;
		auto& recognized_textures = data.Materials.RecognizedTextures;

		std::pmr::polymorphic_allocator alloc(&importData->Arena);
		       set_textures = alloc.new_object<std::pmr::vector<uint32_t>>();
		recognized_textures = alloc.new_object<std::pmr::vector<std::pmr::vector<aiString>>>();
		
		data.Scene = scene;
		data.ImportVariant = ImportVariant::Mesh;
		data.Materials.GLTFTexturePacking = texture_packing;
		data.Materials.PreviewItemSelectedIndex = 0;

		set_textures->resize(scene->mNumMaterials * 6, -1);
		recognized_textures->resize(scene->mNumMaterials);

		for (size_t i = 0; i < scene->mNumMaterials; i++)
		{
			auto& mat = scene->mMaterials[i];
			auto& set_textures_in_material = *(Textures*)&((*set_textures)[i*6]);
			auto& recognized_textures_in_material = (*recognized_textures)[i];

			for (unsigned int j = 1; j <= AI_TEXTURE_TYPE_MAX; j++)
			{
				aiTextureType& texture_type = (aiTextureType&)j;
				if (mat->GetTextureCount(texture_type) > 1)
					FE_LOG_CORE_WARN("No support for layered materials!");

				aiString new_texture;
				if (AI_SUCCESS != mat->GetTexture(texture_type, 0, &new_texture))
					continue;

				// alpha channel handling
				{
					int texture_flags; if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXFLAGS(texture_type, 0), texture_flags))
					{
						int a = 0;
					}

					aiBlendMode blend_mode; if (AI_SUCCESS == mat->Get(AI_MATKEY_BLEND_FUNC(texture_type, 0), blend_mode))
					{
						int a = 0;
					}

					aiString gltf_alphamode; if (AI_SUCCESS == mat->Get(AI_MATKEY_GLTF_ALPHAMODE, gltf_alphamode))
					{
						int a = 0;
					}

					float gltf_aphacutoff; if (AI_SUCCESS == mat->Get(AI_MATKEY_GLTF_ALPHACUTOFF, gltf_aphacutoff))
					{
						int a = 0;
					}
				}
				
				auto new_index = recognized_textures_in_material.size();
				bool found = false;
				for (size_t n=0; n<recognized_textures_in_material.size(); n++)
				{
					if (recognized_textures_in_material[n] == new_texture)
					{
						new_index = n;
						found = true;
						break;
					}
				}

				switch (j)
				{
				case aiTextureType_BASE_COLOR:
					set_textures_in_material.BaseColor = new_index;
					break;
				case aiTextureType_NORMALS:
				case aiTextureType_NORMAL_CAMERA:
					set_textures_in_material.Normal = new_index;
					break;
				case aiTextureType_EMISSION_COLOR:
				case aiTextureType_EMISSIVE:
					set_textures_in_material.Emissive = new_index;
					break;
				default:
					if (texture_packing)
					{
						if (j == aiTextureType_GLTF_METALLIC_ROUGHNESS)
							set_textures_in_material.PackedOMR = new_index;
						break;
					}
					
					switch (j)
					{
					case aiTextureType_AMBIENT_OCCLUSION:
					case aiTextureType_AMBIENT:
					case aiTextureType_LIGHTMAP:
						set_textures_in_material.NonPackedOMR.Occlusion = new_index;
						break;
					case aiTextureType_METALNESS:
						set_textures_in_material.NonPackedOMR.Metalness = new_index;
						break;
					case aiTextureType_DIFFUSE_ROUGHNESS:
						set_textures_in_material.NonPackedOMR.Roughness = new_index;
						break;
					}
				}

				if (!found)
					recognized_textures_in_material.push_back(std::move(new_texture));
			}
		}
	}

	static void ImportAsModel(const std::filesystem::path& filepath, const ImportData* const importData)
	{
		auto scene = GeometryLoader::InspectSourceFile(importData->Filepath);

		FE_CORE_ASSERT(false, "not implemented yet");
		AssetID assetID = NullAssetID;// = AssetManager::CreateAsset();
		AssetHandle<Model> model_handle(assetID);
		auto model_user = model_handle.Use();

		//auto& spec = model_user.GetSpecification();
		//spec.RenderMeshCount = scene->mNumMeshes;

		//std::vector<ImportRenderMesh> render_meshes;
		//
		//for (size_t i = 0; i < scene->mNumMeshes; i++)
		//{
		//	auto& render_mesh = render_meshes.emplace_back();
		//
		//	render_mesh.AssimpMeshIndex = (uint32_t)i;
		//	render_mesh.AssimpMaterialIndex = scene->mMeshes[i]->mMaterialIndex;
		//	render_mesh.IndexCount = scene->mMeshes[i]->mNumFaces * 3;
		//	render_mesh.VertexCount = scene->mMeshes[i]->mNumVertices; 
		//}

		//model_user.SetFilepath(importData->Filepath);

		YAML::Emitter emitter;

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID"              << YAML::Value << model_handle.GetUUID();
		emitter << YAML::Key << "Source File"       << YAML::Value << importData->Filepath.string();
		//emitter << YAML::Key << "Render Mesh Count" << YAML::Value << render_meshes.size();
		emitter << YAML::Key << "Meshes"            << YAML::Value << YAML::BeginSeq;
		//for (auto& renderMesh : render_meshes)
		//{
		//	emitter << YAML::BeginMap;
		//
		//	emitter << YAML::Key << "AssimpMeshIndex" << YAML::Value << renderMesh.AssimpMeshIndex;
		//
		//	//if (importData->GeometryData.ImportMaterials)
		//	//	emitter << YAML::Key << "Assimp Material Index" << YAML::Value << renderMesh.AssimpMaterialIndex;
		//
		//	emitter << YAML::Key << "Index Count"  << YAML::Value << renderMesh.IndexCount;
		//	emitter << YAML::Key << "Vertex Count" << YAML::Value << renderMesh.VertexCount;
		//
		//	emitter << YAML::EndMap;
		//}
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
		auto scene = GeometryLoader::InspectSourceFile(importData->Filepath);
		
		auto y = Project::GetInstance()->AssetsPath;
		auto z = std::filesystem::current_path();
		auto x = filepath.lexically_relative(z);
		auto w = x.lexically_relative(y);
		const AssetID assetID = AssetManager::AssetCreation::ProjectAsset<Mesh>(w);
		{
			auto mesh_user = AssetUser<Mesh>(assetID);

			AssetManager::SetSourcePath(assetID, importData->Filepath.lexically_relative(y));
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

	static void DisplayNode(const aiScene* const scene, const aiNode* const node, uint32_t* meshRowIndex)
	{
		constexpr ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAllColumns;

		ImGui::TableNextRow();
		ImGui::PushID(*meshRowIndex);
		(*meshRowIndex)++;

		ImGui::TableNextColumn();

		if (node->mNumChildren > 0 || node->mNumMeshes > 0)
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
				{
					DisplayNode(scene, node->mChildren[i], meshRowIndex);
				}

				for (size_t i = 0; i < node->mNumMeshes; i++)
				{
					ImGui::TreePush(&i);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextWrapped(scene->mMeshes[node->mMeshes[i]]->mName.C_Str());
					ImGui::TreePop();
				}

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

		ImGui::PopID();
	}

	static void RenderHierarchy(const aiScene* const scene)
	{
		constexpr ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

		if (ImGui::BeginTable("NodesTable", 3, flags))
		{
			uint32_t mesh_row_index = 0;

			//ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Transform", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Mesh Count", ImGuiTableColumnFlags_NoHide);
			ImGui::TableHeadersRow();

			DisplayNode(scene, scene->mRootNode, &mesh_row_index);

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
				ImGui::PushID(i);

				auto& mesh = scene->mMeshes[i];
				auto matIndex = mesh->mMaterialIndex;
				meshCountPerMaterial[matIndex]++;

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%i", i);
				ImGui::TableNextColumn();
				ImGui::TextWrapped(mesh->mName.C_Str());
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

				ImGui::PopID();
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
				ImGui::PushID(i);
				const auto& mat = scene->mMaterials[i];
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%i", i);
				ImGui::TableNextColumn();
				const auto name = mat->GetName();
				ImGui::Text(name.C_Str());
				ImGui::TableNextColumn();
				ImGui::Text("%i", meshCountPerMaterial[i]);
				ImGui::PopID();
			}

			ImGui::EndTable();
		}
	}

	static void RenderMaterialPreview(ImportData* const importData)
	{
		ImGui::PushID("Material Preview");

		auto& data = importData->GeometryData;
		auto& selected_idx = data.Materials.PreviewItemSelectedIndex;
		auto& scene = data.Scene;
		
		const auto selected_material_name = scene->mMaterials[selected_idx]->GetName();
		Scratchpad sp;
		std::pmr::string preview(std::to_string(selected_idx), &sp);
		preview += ". " + std::pmr::string(selected_material_name.C_Str(), &sp);

		if (ImGui::BeginCombo("Material Selection", preview.c_str()))
		{
			for (size_t n = 0; n < scene->mNumMaterials; n++)
			{
				ImGui::PushID(n);
				const bool is_selected = (selected_idx == n);
				auto name = scene->mMaterials[n]->GetName();

				std::pmr::string label(std::to_string(n), &sp);
				label += ". " + std::pmr::string(name.C_Str());

				if (ImGui::Selectable(label.c_str(), is_selected))
					selected_idx = (uint32_t)n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
				ImGui::PopID();
			}
			ImGui::EndCombo();
		}

		auto& mat = scene->mMaterials[selected_idx];

		ImGui::BeginDisabled();

		ImGui::SeparatorText("Parameters");
		ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoPicker;

		aiBlendMode blend_mode; if (AI_SUCCESS == mat->Get(AI_MATKEY_BLEND_FUNC, blend_mode)) ImGui::InputInt("Blend Mode", (int*)&blend_mode);
		//AI_MATKEY_GLTF_ALPHAMODE

		aiColor3D base_color; if (AI_SUCCESS == mat->Get(AI_MATKEY_BASE_COLOR,     base_color)) ImGui::ColorEdit3("Base Color", (float*)&base_color, flags);
		aiColor3D ambient;    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT,  ambient   )) ImGui::ColorEdit3("Ambient",    (float*)&ambient,    flags);
		aiColor3D emissive;   if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive  )) ImGui::ColorEdit3("Emissive",   (float*)&emissive,   flags);

		float metalness; if (AI_SUCCESS == mat->Get(AI_MATKEY_METALLIC_FACTOR,  metalness )) ImGui::InputFloat("Metalness", &metalness);
		float roughness; if (AI_SUCCESS == mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness )) ImGui::InputFloat("Roughness", &roughness);

		ImGui::SeparatorText("Textures");
		
		auto& recognized_textures = (*data.Materials.RecognizedTextures)[selected_idx];

		for (auto& texture : recognized_textures)
		{
			ImGui::Text(texture.C_Str());
		}
		
		ImGui::EndDisabled();

		ImGui::PopID();
	}

	static void RenderVariantSelection(ImportData* const importData)
	{
		ImGui::SeparatorText("Import as:");

		auto& variant = importData->GeometryData.ImportVariant;

		if (importData->GeometryData.Scene->mNumMaterials > 1)
		{
			ImGui::RadioButton("Model", (int*)&variant, ImportVariant::Model);
		}
		else
		{
			ImGui::BeginDisabled();
			ImGui::RadioButton("Model", (int*)&variant, ImportVariant::Model);
			ImGui::EndDisabled();
		}
		ImGui::SameLine();

		if (importData->GeometryData.Scene->mNumMaterials == 1)
		{
			ImGui::RadioButton("RenderMesh", (int*)&variant, ImportVariant::RenderMesh);
		}
		else
		{
			ImGui::BeginDisabled();
			ImGui::RadioButton("RenderMesh", (int*)&variant, ImportVariant::RenderMesh);
			ImGui::EndDisabled();
		}
		ImGui::SameLine();

		ImGui::RadioButton("Mesh", (int*)&variant, ImportVariant::Mesh);
	}

	static void RenderMaterialsTexturesSettingsOverview(ImportData* const importData)
	{
		ImGui::SeparatorText("Materials' Textures");

		auto& data = importData->GeometryData;

		constexpr ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
		uint32_t column_flags = ImGuiTableColumnFlags_AngledHeader | ImGuiTableColumnFlags_WidthFixed;

		int colums_count = data.Materials.GLTFTexturePacking ? 6 : 8;
		if (ImGui::BeginTable("TexturesTable", colums_count, flags))
		{
			ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Material Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("BaseColor", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Emissive", ImGuiTableColumnFlags_NoHide);
			if (data.Materials.GLTFTexturePacking)
				ImGui::TableSetupColumn("PackedOMR", ImGuiTableColumnFlags_NoHide);
			else
			{
				ImGui::TableSetupColumn("Occlusion", ImGuiTableColumnFlags_NoHide);
				ImGui::TableSetupColumn("Metalness", ImGuiTableColumnFlags_NoHide);
				ImGui::TableSetupColumn("Roughness", ImGuiTableColumnFlags_NoHide);
			}

			ImGui::TableHeadersRow();

			ImGui::BeginDisabled();

			for (size_t i = 0; i < data.Scene->mNumMaterials; i++)
			{
				ImGui::TableNextRow();
				ImGui::PushID(i);

				ImGui::TableNextColumn();
				ImGui::Text("%i", i);

				ImGui::TableNextColumn();
				ImGui::Text(data.Scene->mMaterials[i]->GetName().C_Str());

				auto texture_index = i * 6;
				auto& set_textures = data.Materials.SetTextures;
				auto& set_textures_in_material = *(Textures*)&((*set_textures)[i * 6]);

				ImGui::TableNextColumn();
				bool base_color = set_textures_in_material.BaseColor != -1;
				ImGui::Checkbox("##1", &base_color);
				
				ImGui::TableNextColumn();
				bool normal = set_textures_in_material.Normal != -1;
				ImGui::Checkbox("##2", &normal);

				ImGui::TableNextColumn();
				bool emissive = set_textures_in_material.Emissive != -1;
				ImGui::Checkbox("##3", &emissive);

				if (data.Materials.GLTFTexturePacking)
				{
					ImGui::TableNextColumn();
					bool omr = set_textures_in_material.PackedOMR != -1;
					ImGui::Checkbox("##4", &omr);
				}
				else
				{
					ImGui::TableNextColumn();
					bool occlusion = set_textures_in_material.NonPackedOMR.Occlusion != -1;
					ImGui::Checkbox("##5", &occlusion);

					ImGui::TableNextColumn();
					bool metalness = set_textures_in_material.NonPackedOMR.Metalness != -1;
					ImGui::Checkbox("##6", &metalness);

					ImGui::TableNextColumn();
					bool roughness = set_textures_in_material.NonPackedOMR.Roughness != -1;
					ImGui::Checkbox("##7", &roughness);
				}

				ImGui::PopID();
			}

			ImGui::EndDisabled();

			ImGui::EndTable();
		}
	}

	static void RenderMaterialsTexturesSettings(ImportData* const importData)
	{
		ImGui::PushID("Material Settings");

		ImGui::SeparatorText("Material Settings");

		auto& data = importData->GeometryData;
		auto& scene = data.Scene;
		Scratchpad sp;

		static size_t selected_material_index = 0;
		const auto selected_material_name = scene->mMaterials[selected_material_index]->GetName();

		std::pmr::string preview(std::to_string(selected_material_index), &sp);
		preview += ". " + std::pmr::string(selected_material_name.C_Str(), &sp);

		if (ImGui::BeginCombo("Material Selection", preview.c_str()))
		{
			for (size_t n = 0; n < scene->mNumMaterials; n++)
			{
				ImGui::PushID(n);
				const bool is_selected = (selected_material_index == n);
				auto name = scene->mMaterials[n]->GetName();

				std::pmr::string label(std::to_string(n), &sp);
				label += ". " + std::pmr::string(name.C_Str());

				if (ImGui::Selectable(label.c_str(), is_selected))
					selected_material_index = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
				ImGui::PopID();
			}
			ImGui::EndCombo();
		}

		static const char* texture_types_names[] =
		{
			"BaseColor",
			"Normal",
			"Emissive",
			"PackedOMR",
			"Occlusion",
			"Metalness",
			"Roughness"
		};

		auto& set_textures = *data.Materials.SetTextures;
		auto& recognized_textures = (*data.Materials.RecognizedTextures)[selected_material_index];
		auto texture_index = selected_material_index * 6;

		ImGui::Text("Textures:");
		for (size_t i = 0; i < 7; i++)
		{
			if (i == 3 && !data.Materials.GLTFTexturePacking)
				continue;

			const char* combo_preview_value = set_textures[texture_index] != -1 ? recognized_textures[set_textures[texture_index]].C_Str() : "<none>";

			if (ImGui::BeginCombo(texture_types_names[i], combo_preview_value))
			{
				bool is_none_selected = set_textures[texture_index] == -1;
				if (ImGui::Selectable("<none>", is_none_selected))
					set_textures[texture_index] = -1;

				if (is_none_selected)
					ImGui::SetItemDefaultFocus();

				for (unsigned int n = 0; n < recognized_textures.size(); n++)
				{
					const bool is_selected = (set_textures[texture_index] == n);
					if (ImGui::Selectable(recognized_textures[n].C_Str(), is_selected))
						set_textures[texture_index] = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (i == 3 && data.Materials.GLTFTexturePacking)
				break;

			texture_index++;
		}

		ImGui::PopID();
	}

	void RenderWindow(ImportData* const importData)
	{
		auto& data = importData->GeometryData;
		auto& scene = data.Scene;

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

		ImGui::SetNextItemOpen(true);
		if (ImGui::CollapsingHeader("Import Settings"))
		{
			RenderVariantSelection(importData);

			auto& variant = importData->GeometryData.ImportVariant;

			if (variant == ImportVariant::Model || variant == ImportVariant::RenderMesh)
			{
				RenderMaterialsTexturesSettingsOverview(importData);
				RenderMaterialsTexturesSettings(importData);
			}

			ImGui::SeparatorText("##1");
			if (ImGui::Button("Import As..."))
			{
				Scratchpad sp;
				std::filesystem::path defaultFilepath;
				std::pmr::string filter(&sp);
			
				switch (variant)
				{
				case ImportVariant::Model:		defaultFilepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<Model		>(importData->Filepath, filter);	break;
				case ImportVariant::RenderMesh:	defaultFilepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<RenderMesh	>(importData->Filepath, filter);	break;
				case ImportVariant::Mesh:		defaultFilepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<Mesh		>(importData->Filepath, filter);	break;
				}

				std::filesystem::path newAssetFilepath = FileDialogs::SaveFile(defaultFilepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str(), filter.c_str());

				if (!newAssetFilepath.empty())
				{
					switch (variant)
					{
					case ImportVariant::Model:		ImportAsModel(newAssetFilepath, importData); break;
					case ImportVariant::RenderMesh:	ImportAsRenderMesh(newAssetFilepath, importData); break;
					case ImportVariant::Mesh:		ImportAsMesh(newAssetFilepath, importData); break;
					}
				
					importData->Finished = true;
				}
			}
		}
	}
}