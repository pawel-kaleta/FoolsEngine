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
		const static std::filesystem::path s_extensions_guaranteeing[] = {
			".glb",
			".gltf"
		};
		const std::filesystem::path extension = path.extension();
		return extension == s_extensions_guaranteeing[0] || extension == s_extensions_guaranteeing[1];
	}

	void InitImport(ImportData* const importData)
	{
		auto scene = GeometryLoader::InspectSourceFile(importData->FilepathToImport);
		auto texture_packing = IsGuaranteedStandardTexturePacking(importData->FilepathToImport);

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
				if (transparency > 0.f)
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
				if (std::string_view(gltf_alphamode.C_Str()) == "MASK") material_data.AlphaMode = AlphaMode::Cutout;
				if (std::string_view(gltf_alphamode.C_Str()) == "BLEND") material_data.AlphaMode = AlphaMode::Blend;
			}

			float gltf_alphacutoff; if (AI_SUCCESS == mat->Get(AI_MATKEY_GLTF_ALPHACUTOFF, gltf_alphacutoff))
			{
				material_data.DetectedProperties |= DetectedMaterialProperties::AlphaCutoff;
				uniforms.AlphaCutoff = gltf_alphacutoff;
				if (gltf_alphacutoff < 1.f && material_data.AlphaMode == AlphaMode::Opaque)
				{
					material_data.AlphaMode = AlphaMode::Cutout;
				}
			}
			else
				uniforms.AlphaCutoff = 0;

			material_data.DetectedTextures = alloc.new_object<Xar<aiString>>(alloc);

			material_data.RecognizedTextures.BaseColor = -1;
			material_data.RecognizedTextures.Emissive = -1;
			material_data.RecognizedTextures.Normal = -1;
			material_data.RecognizedTextures.NonPackedORM.Metalness = -1;
			material_data.RecognizedTextures.NonPackedORM.Occlusion = -1;
			material_data.RecognizedTextures.NonPackedORM.Roughness = -1;

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
							FE_CORE_ASSERT(material_data.AlphaMode);
							material_data.AlphaMode = AlphaMode::Blend;
						}
						if (aiTextureFlags::aiTextureFlags_IgnoreAlpha & texture_flags)
						{
							FE_CORE_ASSERT(!material_data.AlphaMode);
							material_data.AlphaMode = AlphaMode::Opaque;
						}
					}
				}

				uint32_t new_index = (uint32_t)material_data.DetectedTextures->Size();
				bool found = false;
				for (size_t n = 0; n < material_data.DetectedTextures->Size(); n++)
				{
					if (material_data.DetectedTextures->operator[](n) == new_texture)
					{
						new_index = (uint32_t)n;
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
							material_data.RecognizedTextures.PackedORM = new_index;
						break;
					}

					switch (texture_type)
					{
					case aiTextureType_AMBIENT_OCCLUSION:
					case aiTextureType_AMBIENT:
					case aiTextureType_LIGHTMAP:
						material_data.RecognizedTextures.NonPackedORM.Occlusion = new_index;
						break;
					case aiTextureType_METALNESS:
						material_data.RecognizedTextures.NonPackedORM.Metalness = new_index;
						break;
					case aiTextureType_DIFFUSE_ROUGHNESS:
						material_data.RecognizedTextures.NonPackedORM.Roughness = new_index;
						break;
					}
				}
			}
		}
	}

	static void CreateTextureForMaterial(const char* textureSlotName, ACMaterialCore& core, const aiString& filePath, const AssetUser<Material>& materialUser, const ImportData* const importData, TextureData::Usage usage)
	{
		AssetID textureID = AssetManager::AssetCreation::InternalAsset<Texture2D>(materialUser.GetID());
		AssetHandle<Texture2D> texture_handle(textureID);

		auto full_texture_path = importData->FilepathToImport.parent_path() / std::filesystem::path(filePath.C_Str());

		auto& spec = texture_handle.Use().GetCoreComponent().Specification;
		spec = TextureLoader::InspectTexture(full_texture_path);
		spec.Usage = usage;
		AssetManager::SetSourcePath(textureID, filePath.C_Str());
		materialUser.SetTexture(core, textureSlotName, textureID);
	}

	static void CreateBaseMaterial(const AssetUser<Material>& materialUser, GeometryImport::MaterialData& materialData, const ImportData* const importData)
	{
		auto& core = materialUser.GetCoreComponent();

		if (materialData.DetectedProperties & DetectedMaterialProperties::Roughness)
			materialUser.SetUniformValue(core, "u_Roughness", &materialData.Uniforms.Roughness);

		if (materialData.DetectedProperties & DetectedMaterialProperties::Metalness)
			materialUser.SetUniformValue(core, "u_Metalness", &materialData.Uniforms.Metalness);

		if (materialData.DetectedProperties & DetectedMaterialProperties::Ambient)
			materialUser.SetUniformValue(core, "u_AO", &materialData.Uniforms.Ambient);

		auto& all = *materialData.DetectedTextures;
		auto& recognized = materialData.RecognizedTextures;

		if (recognized.BaseColor != -1)
			CreateTextureForMaterial("u_BaseColorMap", core, all[recognized.BaseColor], materialUser, importData, TextureData::Usage::Map_BaseColor);
		else
			materialUser.SetTexture(core, "u_BaseColorMap", NullAssetID);

		auto& texture_packing = importData->GeometryData.GLTFTexturePacking;
		materialUser.SetUniformValue(core, "u_ORMTexturePacking", (void*)&texture_packing);

		if (texture_packing)
		{
			if (recognized.PackedORM != -1)
				CreateTextureForMaterial("u_ORMMap", core, all[recognized.PackedORM], materialUser, importData, TextureData::Usage::Map_ORM);
			else
				materialUser.SetTexture(core, "u_ORMMap", NullAssetID);

			materialUser.SetTexture(core, "u_RoughnessMap", NullAssetID);
			materialUser.SetTexture(core, "u_MetalnessMap", NullAssetID);
			materialUser.SetTexture(core, "u_AOMap", NullAssetID);
		}
		else
		{
			materialUser.SetTexture(core, "u_ORMMap", NullAssetID);

			if (recognized.NonPackedORM.Roughness != -1)
				CreateTextureForMaterial("u_RoughnessMap", core, all[recognized.NonPackedORM.Roughness], materialUser, importData, TextureData::Usage::Map_Roughness);
			else
				materialUser.SetTexture(core, "u_RoughnessMap", NullAssetID);

			if (recognized.NonPackedORM.Metalness != -1)
				CreateTextureForMaterial("u_MetalnessMap", core, all[recognized.NonPackedORM.Metalness], materialUser, importData, TextureData::Usage::Map_Metalness);
			else
				materialUser.SetTexture(core, "u_MetalnessMap", NullAssetID);

			if (recognized.NonPackedORM.Occlusion != -1)
				CreateTextureForMaterial("u_AOMap", core, all[recognized.NonPackedORM.Occlusion], materialUser, importData, TextureData::Usage::Map_AO);
			else
				materialUser.SetTexture(core, "u_AOMap", NullAssetID);
		}

		if (recognized.Normal != -1)
			CreateTextureForMaterial("u_NormalMap", core, all[recognized.Normal], materialUser, importData, TextureData::Usage::Map_Normal);
		else
			materialUser.SetTexture(core, "u_NormalMap", NullAssetID);
	}

	static void ImportAsModel(const std::filesystem::path& filepath, const ImportData* const importData)
	{
		auto& scene = importData->GeometryData.Scene;

		auto assets_path = Project::Get()->m_AssetsPath;
		auto x = filepath.lexically_relative(std::filesystem::current_path());
		auto w = x.lexically_relative(assets_path);

		AssetID assetID = AssetManager::AssetCreation::ProjectAsset<Model>(w);
		AssetManager::SetSourcePath(assetID, importData->FilepathToImport.lexically_relative(assets_path));

		{
			AssetUser<Model> model_user(assetID);
			auto& model_core = model_user.GetCoreComponent();

			Scratchpad sp;
			std::pmr::vector<AssetID> material_IDs(&sp);

			material_IDs.reserve(scene->mNumMaterials);

			for (size_t i = 0; i < scene->mNumMaterials; i++)
			{
				AssetID material_ID = AssetManager::AssetCreation::InternalAsset<Material>(assetID);
				material_IDs.push_back(material_ID);

				auto& material_data = importData->GeometryData.MaterialsData->operator[](i);

				AssetUser<Material> material_user(material_ID);
				auto& material_core = material_user.GetCoreComponent();

				if (material_data.AlphaMode == AlphaMode::Opaque)
				{
					material_user.MakeMaterial(Renderer::BaseAssets.ShadingModels.Base3DOpaque.Observe());
					CreateBaseMaterial(material_user, material_data, importData);

					if (material_data.DetectedProperties & DetectedMaterialProperties::BaseColor)
						material_user.SetUniformValue(material_core, "u_BaseColor", &material_data.Uniforms.BaseColor);
				}
				else
				{
					material_user.MakeMaterial(Renderer::BaseAssets.ShadingModels.Base3DBlend.Observe());
					CreateBaseMaterial(material_user, material_data, importData);

					glm::vec4 base_color = { 1.f, 1.f, 1.f, 1.f };

					if (material_data.DetectedProperties & DetectedMaterialProperties::BaseColor)
						base_color = { material_data.Uniforms.BaseColor, 1.f };
					if (material_data.DetectedProperties & DetectedMaterialProperties::Opacity)
						base_color.a = material_data.Uniforms.Opacity;
					material_user.SetUniformValue(material_core, "u_BaseColor", &material_data.Uniforms.BaseColor);

					if (material_data.DetectedProperties & DetectedMaterialProperties::AlphaCutoff)
						material_user.SetUniformValue(material_core, "u_AlphaCutOff", &material_data.Uniforms.AlphaCutoff);
				}
			}

			for (size_t i = 0; i < scene->mNumMeshes; i++)
			{
				AssetID mesh_ID = AssetManager::AssetCreation::InternalAsset<Mesh>(assetID);

				auto mesh_user = AssetUser<Mesh>(mesh_ID);
				auto& mesh_core = mesh_user.GetCoreComponent();

				mesh_core.Specification.VertexCount = scene->mMeshes[i]->mNumVertices;
				mesh_core.Specification.IndexCount = scene->mMeshes[i]->mNumFaces * 3;

				AssetID render_mesh_ID = AssetManager::AssetCreation::InternalAsset<RenderMesh>(assetID);
				model_core.RenderMeshIDs.emplace_back(render_mesh_ID);

				auto render_mesh_user = AssetUser<RenderMesh>(render_mesh_ID);
				auto& render_mesh_core = render_mesh_user.GetCoreComponent();

				auto& material_index = scene->mMeshes[i]->mMaterialIndex;
				auto& material_ID = material_IDs[material_index];

				render_mesh_core.MeshID = mesh_ID;
				render_mesh_core.MaterialID = material_ID;
			}
		}

		YAML::Emitter emitter;
		Model::SaveMetadata(emitter, assetID);
		std::ofstream fout(Project::Get()->m_AssetsPath / AssetObserver<Model>(assetID).GetFilepath());
		fout << emitter.c_str();

		AssetSerializer::SerializeRegistry();
	}

	static void ImportAsRenderMesh(const std::filesystem::path& targetFilepath, const ImportData* const importData)
	{

	}

	static void ImportAsMesh(const std::filesystem::path& targetFilepath, const ImportData* const importData)
	{
		auto& scene = importData->GeometryData.Scene;

		auto y = Project::Get()->m_AssetsPath;
		auto z = std::filesystem::current_path();
		auto x = targetFilepath.lexically_relative(z);
		auto w = x.lexically_relative(y);
		const AssetID assetID = AssetManager::AssetCreation::ProjectAsset<Mesh>(w);
		{
			auto mesh_user = AssetUser<Mesh>(assetID);

			AssetManager::SetSourcePath(assetID, importData->FilepathToImport.lexically_relative(y));
			auto& core = mesh_user.GetCoreComponent();
			auto& specification = core.Specification;

			for (size_t i = 0; i < scene->mNumMeshes; i++)
			{
				specification.VertexCount += scene->mMeshes[i]->mNumVertices;
				specification.IndexCount += scene->mMeshes[i]->mNumFaces;
			}
			specification.IndexCount *= 3;
		}

		YAML::Emitter emitter;
		Mesh::SaveMetadata(emitter, assetID);
		std::ofstream fout(Project::Get()->m_AssetsPath / AssetObserver<Mesh>(assetID).GetFilepath());
		fout << emitter.c_str();

		AssetSerializer::SerializeRegistry();
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
				ImGui::PushID((int)i);

				auto& mesh = scene->mMeshes[i];
				auto material_index = mesh->mMaterialIndex;
				meshCountPerMaterial[material_index]++;

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%i", i);
				ImGui::TableNextColumn();
				ImGui::TextWrapped(mesh->mName.C_Str());
				ImGui::TableNextColumn();
				const auto name = scene->mMaterials[material_index]->GetName();
				std::pmr::string nameLabel(std::to_string(material_index), &sp);
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
				ImGui::PushID((int)i);
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
				ImGui::TableSetupColumn("PackedORM", ImGuiTableColumnFlags_NoHide);
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
				ImGui::PushID((int)i);

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
					bool orm = recognized_textures.PackedORM != -1;
					ImGui::Checkbox("##4", &orm);
				}
				else
				{
					ImGui::TableNextColumn();
					bool occlusion = recognized_textures.NonPackedORM.Occlusion != -1;
					ImGui::Checkbox("##5", &occlusion);

					ImGui::TableNextColumn();
					bool metalness = recognized_textures.NonPackedORM.Metalness != -1;
					ImGui::Checkbox("##6", &metalness);

					ImGui::TableNextColumn();
					bool roughness = recognized_textures.NonPackedORM.Roughness != -1;
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
				ImGui::PushID((int)n);
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

		ImGui::InputFloat("Roughness##uniform", &uniforms.Roughness);
		ImGui::InputFloat("Metalness##uniform", &uniforms.Metalness);

		if (props & DetectedMaterialProperties::AlphaCutoff ||
			props & DetectedMaterialProperties::Opacity ||
			material_data.AlphaMode & AlphaMode::Blend ||
			material_data.AlphaMode & AlphaMode::Cutout)
		{
			ImGui::InputFloat("Transparency##uniform", &uniforms.Opacity);
			ImGui::InputFloat("AlphaCutoff##uniform", &uniforms.AlphaCutoff);
		}

		ImGui::EndDisabled();

		static const char* s_texture_types_names[] =
		{
			"BaseColor",
			"Normal",
			"Emissive",
			"PackedORM",
			"Occlusion",
			"Roughness",
			"Metalness"
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

			if (ImGui::BeginCombo(s_texture_types_names[i], combo_preview_value))
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
		ImGui::Text("m_File: %s", importData->FilepathToImport.string().c_str());

		if (ImGui::CollapsingHeader("Nodes", 0))
			RenderHierarchy(scene);

		{
			Scratchpad sp;
			std::pmr::vector<uint32_t> meshes_counts_per_material(scene->mNumMaterials, 0, &sp);

			if (ImGui::CollapsingHeader("Meshes", 0))
				RenderMeshList(meshes_counts_per_material, scene);
			else
			{
				for (size_t i = 0; i < scene->mNumMeshes; i++)
				{
					auto material_index = scene->mMeshes[i]->mMaterialIndex;
					meshes_counts_per_material[material_index]++;
				}
			}

			if (ImGui::CollapsingHeader("Materials"))
				RenderMaterialList(meshes_counts_per_material, scene);
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
				std::filesystem::path default_filepath;
				std::pmr::string filter(&sp);

				switch (variant)
				{
				case ImportVariant::Model:		default_filepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<Model		>(importData->FilepathToImport, filter);	break;
				case ImportVariant::RenderMesh:	default_filepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<RenderMesh	>(importData->FilepathToImport, filter);	break;
				case ImportVariant::Mesh:		default_filepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<Mesh		>(importData->FilepathToImport, filter);	break;
				}

				std::filesystem::path newAssetFilepath = FileDialogs::SaveFile(default_filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str(), filter.c_str());

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