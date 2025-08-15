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

		std::pmr::polymorphic_allocator alloc(&importData->Arena);
		data.MaterialsData = alloc.new_object<std::pmr::vector<MaterialData>>();
		
		data.Scene = scene;
		data.ImportVariant = ImportVariant::Mesh;
		data.GLTFTexturePacking = texture_packing;
		data.PreviewItemSelectedIndex = 0;

		data.MaterialsData->resize(scene->mNumMaterials);

		for (size_t i = 0; i < scene->mNumMaterials; i++)
		{
			auto& mat = scene->mMaterials[i];
			auto& material_data = data.MaterialsData->operator[](i);
			auto& uniforms = material_data.Uniforms;

			material_data.DetectedProperties = 0;

			aiColor3D base_color; if (AI_SUCCESS == mat->Get(AI_MATKEY_BASE_COLOR, base_color))
			{
				material_data.DetectedProperties |= DetectedMaterialProperties::BaseColor;
				uniforms.BaseColor = { base_color.r, base_color.g, base_color.b };
			}
			else
				uniforms.BaseColor = { 1.f, 1.f, 1.f };

			aiColor3D ambient; if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
			{
				material_data.DetectedProperties |= DetectedMaterialProperties::Ambient;
				uniforms.Ambient = { ambient.r, ambient.g, ambient.b };
			}
			else
				uniforms.Ambient = { 1.f, 1.f, 1.f };

			aiColor3D emissive; if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive))
			{
				material_data.DetectedProperties |= DetectedMaterialProperties::Emissive;
				uniforms.Emissive = { emissive.r, emissive.g, emissive.b };
			}
			else
				uniforms.Emissive = { 0.f, 0.f, 0.f };

			uniforms.Opacity = 1.f;
			float transparency; if (AI_SUCCESS == mat->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparency))
			{
				material_data.DetectedProperties |= DetectedMaterialProperties::Opacity;
				uniforms.Opacity = 1.f - transparency;
				if (transparency < 1.f)
					material_data.AlphaMode = AlphaMode::Blend;
			}

			float opacity; if (AI_SUCCESS == mat->Get(AI_MATKEY_OPACITY, opacity))
			{
				material_data.DetectedProperties |= DetectedMaterialProperties::Opacity;
				uniforms.Opacity = opacity;
				if (transparency < 1.f)
					material_data.AlphaMode = AlphaMode::Blend;
			}


			aiString gltf_alphamode; if (AI_SUCCESS == mat->Get(AI_MATKEY_GLTF_ALPHAMODE, gltf_alphamode))
			{
				if (std::string_view(gltf_alphamode.C_Str()) == "OPAQUE") material_data.AlphaMode = AlphaMode::Opaque;
				if (std::string_view(gltf_alphamode.C_Str()) == "MASK"  ) material_data.AlphaMode = AlphaMode::Mask;
				if (std::string_view(gltf_alphamode.C_Str()) == "BLEND" ) material_data.AlphaMode = AlphaMode::Blend;
			}

			float gltf_alphacutoff; if (AI_SUCCESS == mat->Get(AI_MATKEY_GLTF_ALPHACUTOFF, gltf_alphacutoff))
			{
				material_data.DetectedProperties |= DetectedMaterialProperties::AlphaCutoff;
				uniforms.AlphaCutoff = gltf_alphacutoff;
			}
			else
				uniforms.AlphaCutoff = 0;

			material_data.DetectedTextures = alloc.new_object<Xar<aiString>>(alloc);

			material_data.RecognizedTextures.BaseColor = -1;
			material_data.RecognizedTextures.Emissive = -1;
			material_data.RecognizedTextures.Normal = -1;
			material_data.RecognizedTextures.NonPackedOMR.Metalness = -1;
			material_data.RecognizedTextures.NonPackedOMR.Occlusion = -1;
			material_data.RecognizedTextures.NonPackedOMR.Roughness = -1;

			for (unsigned int j = 1; j <= AI_TEXTURE_TYPE_MAX; j++)
			{
				aiTextureType& texture_type = (aiTextureType&)j;
				if (mat->GetTextureCount(texture_type) > 1)
					FE_LOG_CORE_WARN("No support for layered materials!");

				aiString new_texture;
				if (AI_SUCCESS != mat->GetTexture(texture_type, 0, &new_texture))
					continue;

				// alpha channel handling
				if (texture_type == aiTextureType_BASE_COLOR)
				{
					int texture_flags; if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXFLAGS(texture_type, 0), texture_flags))
					{
						if (aiTextureFlags::aiTextureFlags_UseAlpha & texture_flags)
						{
							FE_CORE_ASSERT(material_data.AlphaMode)
							material_data.AlphaMode = AlphaMode::Blend;
						}
						if (aiTextureFlags::aiTextureFlags_IgnoreAlpha & texture_flags)
						{
							material_data.AlphaMode = AlphaMode::Opaque;
						}
					}
				}
				
				auto new_index = material_data.DetectedTextures->Size();
				bool found = false;
				for (size_t n=0; n<material_data.DetectedTextures->Size(); n++)
				{
					if (material_data.DetectedTextures->operator[](n) == new_texture)
					{
						new_index = n;
						found = true;
						break;
					}
				}

				if (!found)
					material_data.DetectedTextures->PushBack(std::move(new_texture));

				switch (texture_type)
				{
				case aiTextureType_BASE_COLOR:
					material_data.RecognizedTextures.BaseColor = new_index;
					break;
				case aiTextureType_NORMALS:
				case aiTextureType_NORMAL_CAMERA:
					material_data.RecognizedTextures.Normal = new_index;
					break;
				case aiTextureType_EMISSION_COLOR:
				case aiTextureType_EMISSIVE:
					material_data.RecognizedTextures.Emissive = new_index;
					break;
				default:
					if (texture_packing)
					{
						if (texture_type == aiTextureType_GLTF_METALLIC_ROUGHNESS)
							material_data.RecognizedTextures.PackedOMR = new_index;
						break;
					}
					
					switch (texture_type)
					{
					case aiTextureType_AMBIENT_OCCLUSION:
					case aiTextureType_AMBIENT:
					case aiTextureType_LIGHTMAP:
						material_data.RecognizedTextures.NonPackedOMR.Occlusion = new_index;
						break;
					case aiTextureType_METALNESS:
						material_data.RecognizedTextures.NonPackedOMR.Metalness = new_index;
						break;
					case aiTextureType_DIFFUSE_ROUGHNESS:
						material_data.RecognizedTextures.NonPackedOMR.Roughness = new_index;
						break;
					}
				}
			}
		}
	}

	static void CreateOpaqueMaterial(AssetID materialID, GeometryImport::MaterialData& materialData, const aiScene* scene)
	{
		AssetUser<Material> material_user(materialID);
		auto& core = material_user.GetCoreComponent();

		material_user.MakeMaterial(Renderer::BaseAssets.ShadingModels.Default.Observe());

		if (materialData.DetectedProperties & DetectedMaterialProperties::BaseColor)
			material_user.SetUniformValue(core, "u_BaseColor", &materialData.Uniforms.BaseColor);

		if (materialData.DetectedProperties & DetectedMaterialProperties::Roughness)
			material_user.SetUniformValue(core, "u_Roughness", &materialData.Uniforms.Roughness);

		if (materialData.DetectedProperties & DetectedMaterialProperties::Metalness)
			material_user.SetUniformValue(core, "u_Metalness", &materialData.Uniforms.Metalness);

		if (materialData.DetectedProperties & DetectedMaterialProperties::Ambient)
			material_user.SetUniformValue(core, "u_AO", &materialData.Uniforms.Ambient);

		auto& all = *materialData.DetectedTextures;
		auto& recognized = materialData.RecognizedTextures;

		if (recognized.BaseColor != -1)
		{
			AssetID textureID = AssetManager::AssetCreation::InternalAsset<Texture2D>(materialID);
			AssetHandle<Texture2D> textureHandle(textureID);
			
			auto& texture_path = all[recognized.BaseColor];
			textureHandle.Use().GetCoreComponent().Specification = TextureLoader::InspectTexture(texture_path.C_Str());
			AssetManager::SetSourcePath(textureID, texture_path.C_Str());
			material_user.SetTexture(core, "u_BaseColorMap", textureID);
		}
		else
		{
			material_user.SetTexture(core, "u_BaseColorMap", Renderer::BaseAssets.Textures.FlatWhite.GetID());
		}

		if (recognized.PackedOMR != -1)
		{
			AssetID textureID = AssetManager::AssetCreation::InternalAsset<Texture2D>(materialID);
			AssetHandle<Texture2D> textureHandle(textureID);

			auto& texture_path = all[recognized.PackedOMR];
			textureHandle.Use().GetCoreComponent().Specification = TextureLoader::InspectTexture(texture_path.C_Str());
			AssetManager::SetSourcePath(textureID, texture_path.C_Str());
			material_user.SetTexture(core, "u_OMRMap", textureID);
		}
		else
		{
			material_user.SetTexture(core, "u_OMRMap", Renderer::BaseAssets.Textures.FlatWhite.GetID());
		}

		if (recognized.Normal != -1)
		{
			AssetID textureID = AssetManager::AssetCreation::InternalAsset<Texture2D>(materialID);
			AssetHandle<Texture2D> textureHandle(textureID);

			auto& texture_path = all[recognized.Normal];
			textureHandle.Use().GetCoreComponent().Specification = TextureLoader::InspectTexture(texture_path.C_Str());
			AssetManager::SetSourcePath(textureID, texture_path.C_Str());
			material_user.SetTexture(core, "u_NormalMap", textureID);
		}
		else
		{
			material_user.SetTexture(core, "u_NormalMap", Renderer::BaseAssets.Textures.FlatWhite.GetID());
		}
	}

	static void ImportAsModel(const std::filesystem::path& filepath, const ImportData* const importData)
	{
		auto& scene = importData->GeometryData.Scene;

		auto assets_path = Project::GetInstance()->AssetsPath;
		auto x = filepath.lexically_relative(std::filesystem::current_path());
		auto w = x.lexically_relative(assets_path);

		AssetID assetID = AssetManager::AssetCreation::ProjectAsset<Model>(w);
		AssetManager::SetSourcePath(assetID, importData->Filepath.lexically_relative(assets_path));
		AssetHandle<Model> model_handle(assetID);
		auto model_user = model_handle.Use();

		auto& core = model_user.GetCoreComponent();

		Scratchpad sp;
		std::pmr::vector<AssetID> mesh_IDs(&sp);
		std::pmr::vector<AssetID> material_IDs(&sp);

		mesh_IDs.reserve(scene->mNumMeshes);
		material_IDs.reserve(scene->mNumMaterials);

		for (size_t i = 0; i < scene->mNumMaterials; i++)
		{
			AssetID material_ID = AssetManager::AssetCreation::InternalAsset<Material>(assetID);
			material_IDs.push_back(material_ID);

			auto& material_data = importData->GeometryData.MaterialsData->operator[](i);
			
			if (material_data.AlphaMode == AlphaMode::Opaque)
			{
				CreateOpaqueMaterial(material_ID, material_data, scene);
			}
			else
			{
				FE_LOG_CORE_WARN("Unimplemented default alpha material in geometry import");
				//material_user.MakeMaterial(Renderer::BaseAssets.ShadingModels.Default.Observe()); // alpha in future
			}

			auto material = scene->mMaterials[i];
		}

		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			AssetID mesh_ID = AssetManager::AssetCreation::InternalAsset<Mesh>(assetID);
			mesh_IDs.push_back(mesh_ID);
		}

		{
			AssetID render_mesh_ID = AssetManager::AssetCreation::InternalAsset<RenderMesh>(assetID);
			auto& render_mesh = core.RenderMeshes.emplace_back(render_mesh_ID, AssetLoadingPriority::None);
		
			auto render_mesh_user = render_mesh.Use();
			auto& render_mesh_core = render_mesh_user.GetCoreComponent();

			
			render_mesh_core.MeshHandle = AssetHandle<Mesh>(mesh_ID);
			auto mesh_user = AssetUser<Mesh>(mesh_ID);
			auto& core = mesh_user.GetCoreComponent();
			auto& specification = core.Specification;

			scene->mMaterials[i]->
			for (size_t i = 0; i < scene->mNumMeshes; i++)
			{
				specification.VertexCount += scene->mMeshes[i]->mNumVertices;
				specification.IndexCount += scene->mMeshes[i]->mNumFaces;
			}
			specification.IndexCount *= 3;
			

			Mesh::SaveMetadata(assetID);

			AssetID material_ID = AssetManager::AssetCreation::InternalAsset<Material>(assetID);
			render_mesh_core.MaterialHandle = AssetHandle<Material>(material_ID);
			render_mesh_core.MaterialHandle.Use().GetCoreComponent().

			render_mesh.AssimpMeshIndex = (uint32_t)i;
			render_mesh.AssimpMaterialIndex = scene->mMeshes[i]->mMaterialIndex;
			render_mesh.IndexCount = scene->mMeshes[i]->mNumFaces * 3;
			render_mesh.VertexCount = scene->mMeshes[i]->mNumVertices; 
		}


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
		auto& scene = importData->GeometryData.Scene;
		
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

		int colums_count = data.GLTFTexturePacking ? 6 : 8;
		if (ImGui::BeginTable("TexturesTable", colums_count, flags))
		{
			ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Material Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("BaseColor", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Emissive", ImGuiTableColumnFlags_NoHide);
			if (data.GLTFTexturePacking)
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

				auto& materials_data = data.MaterialsData->operator[](i);
				auto& recognized_textures = materials_data.RecognizedTextures;

				ImGui::TableNextColumn();
				bool base_color = recognized_textures.BaseColor != -1;
				ImGui::Checkbox("##1", &base_color);
				
				ImGui::TableNextColumn();
				bool normal = recognized_textures.Normal != -1;
				ImGui::Checkbox("##2", &normal);

				ImGui::TableNextColumn();
				bool emissive = recognized_textures.Emissive != -1;
				ImGui::Checkbox("##3", &emissive);

				if (data.GLTFTexturePacking)
				{
					ImGui::TableNextColumn();
					bool omr = recognized_textures.PackedOMR != -1;
					ImGui::Checkbox("##4", &omr);
				}
				else
				{
					ImGui::TableNextColumn();
					bool occlusion = recognized_textures.NonPackedOMR.Occlusion != -1;
					ImGui::Checkbox("##5", &occlusion);

					ImGui::TableNextColumn();
					bool metalness = recognized_textures.NonPackedOMR.Metalness != -1;
					ImGui::Checkbox("##6", &metalness);

					ImGui::TableNextColumn();
					bool roughness = recognized_textures.NonPackedOMR.Roughness != -1;
					ImGui::Checkbox("##7", &roughness);
				}

				ImGui::PopID();
			}

			ImGui::EndDisabled();

			ImGui::EndTable();
		}
	}

	static void RenderMaterialSettings(ImportData* const importData)
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

		auto& material_data = data.MaterialsData->operator[](selected_material_index);
		auto& uniforms = material_data.Uniforms;
		auto& props = material_data.DetectedProperties;

		ImGui::SeparatorText("Parameters");
		ImGui::BeginDisabled();

		ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoPicker;

		ImGui::ColorEdit3("Base Color##uniform", (float*)&uniforms.BaseColor, flags);
		ImGui::ColorEdit3("Ambient##uniform", (float*)&uniforms.Ambient, flags);
		ImGui::ColorEdit3("Emissive##uniform", (float*)&uniforms.Emissive, flags);

		ImGui::InputFloat("Metalness##uniform", &uniforms.Metalness);
		ImGui::InputFloat("Roughness##uniform", &uniforms.Roughness);

		if (props & DetectedMaterialProperties::AlphaCutoff ||
			props & DetectedMaterialProperties::Opacity ||
			material_data.AlphaMode & AlphaMode::Blend ||
			material_data.AlphaMode & AlphaMode::Mask)
		{
			ImGui::InputFloat("Transparency##uniform", &uniforms.Opacity);
			ImGui::InputFloat("AlphaCutoff##uniform", &uniforms.AlphaCutoff);
		}

		ImGui::EndDisabled();

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

		auto& recognized_textures = material_data.RecognizedTextures;
		auto& detected_textures = *(material_data.DetectedTextures);

		ImGui::Text("Textures:");
		uint32_t* recognized_texture_ptr = &(recognized_textures.BaseColor);
		for (size_t i = 0; i < 7; i++)
		{
			if (i == 3 && !data.GLTFTexturePacking)
				continue;

			const char* combo_preview_value = *recognized_texture_ptr != -1 ? detected_textures[*recognized_texture_ptr].C_Str() : "<none>";

			if (ImGui::BeginCombo(texture_types_names[i], combo_preview_value))
			{
				bool is_none_selected = *recognized_texture_ptr == -1;
				if (ImGui::Selectable("<none>", is_none_selected))
					*recognized_texture_ptr = -1;

				if (is_none_selected)
					ImGui::SetItemDefaultFocus();

				for (unsigned int n = 0; n < detected_textures.Size(); n++)
				{
					const bool is_selected = (*recognized_texture_ptr == n);
					if (ImGui::Selectable(detected_textures[n].C_Str(), is_selected))
						*recognized_texture_ptr = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (i == 3 && data.GLTFTexturePacking)
				break;

			recognized_texture_ptr++;
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
		}

		ImGui::SetNextItemOpen(true);
		if (ImGui::CollapsingHeader("Import Settings"))
		{
			RenderVariantSelection(importData);

			auto& variant = importData->GeometryData.ImportVariant;

			if (variant == ImportVariant::Model || variant == ImportVariant::RenderMesh)
			{
				RenderMaterialsTexturesSettingsOverview(importData);
				RenderMaterialSettings(importData);
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