#include "FE_pch.h"
#include "Model.h"

#include "RenderMesh.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

#include "FoolsEngine\Core\Project.h"

namespace fe
{
	extern void EmitShaderDataType(YAML::Emitter& emitter, char* dataPtr, const ShaderData::Type& type);

	void Model::SaveMetadata(AssetID assetID)
	{
		Scratchpad sp;

		AssetObserver<Model> assetObserver(assetID);
		auto& model_core = assetObserver.GetCoreComponent();

		YAML::Emitter emitter;

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << assetObserver.GetUUID();
		emitter << YAML::Key << "Filepath" << YAML::Value << assetObserver.GetSourceFilepath()->Filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
		emitter << YAML::Key << "RenderMeshes" << YAML::Value << YAML::BeginSeq;

		for (const auto& renderMeshID : model_core.RenderMeshIDs)
		{
			AssetObserver<RenderMesh> renderMesh_observer(renderMeshID);
			auto& renderMesh_core = renderMesh_observer.GetCoreComponent();

			emitter << YAML::BeginMap;
			emitter << YAML::Key << "UUID" << YAML::Value << renderMesh_observer.GetUUID();
			emitter << YAML::Key << "Mesh" << YAML::Value << YAML::BeginMap;
			{
				AssetObserver<Mesh> mesh_observer(renderMesh_core.MeshID);
				auto& mesh_core = mesh_observer.GetCoreComponent();

				emitter << YAML::Key << "UUID" << YAML::Value << mesh_observer.GetUUID();
				emitter << YAML::Key << "Vartex Count" << YAML::Value << mesh_core.Specification.VertexCount;
				emitter << YAML::Key << "Index Count" << YAML::Value << mesh_core.Specification.IndexCount;
			}
			emitter << YAML::EndMap;
			emitter << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;
			{
				AssetObserver<Material> material_observer(renderMesh_core.MaterialID);
				auto& material_core = material_observer.GetCoreComponent();
				emitter << YAML::Key << "UUID" << YAML::Value << material_observer.GetUUID();

				AssetObserver<ShadingModel> shading_model_observer(material_core.ShadingModelID);
				const auto& shading_model_core = shading_model_observer.GetCoreComponent();

				emitter << YAML::Key << "Shading Model" << YAML::Value << shading_model_observer.GetUUID();
				emitter << YAML::Key << "Uniforms Data Size" << YAML::Value << material_core.UniformsDataSize;
				emitter << YAML::Key << "Uniforms" << YAML::Value << YAML::BeginSeq;

				char* uniform_data_ptr = (char*)material_core.UniformsData;
				for (auto& uniform : shading_model_core.Uniforms)
				{
					emitter << YAML::BeginMap;
					emitter << YAML::Key << "Name" << YAML::Value << uniform.GetName();
					emitter << YAML::Key << "Type" << YAML::Value << uniform.GetType().ToConstCharPtr();
					emitter << YAML::Key << "Count" << YAML::Value << uniform.GetCount();
					emitter << YAML::Key << "Value" << YAML::Value << YAML::BeginSeq;

					auto type = uniform.GetType();
					for (size_t i = 0; i < uniform.GetCount(); i++)
					{
						float* test = (float*)uniform_data_ptr;
						EmitShaderDataType(emitter, uniform_data_ptr, type);
						uniform_data_ptr += uniform.GetSize();
					}
					emitter << YAML::EndSeq;
					emitter << YAML::EndMap;
				}
				emitter << YAML::EndSeq;

				emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;
				for (size_t i = 0; i < shading_model_core.TextureSlots.size(); ++i)
				{
					emitter << YAML::BeginMap;
					emitter << YAML::Key << "Shader Texture Slot" << YAML::Value << shading_model_core.TextureSlots[i].GetName();
					if (material_core.TextureIDs[i] != NullAssetID)
					{
						const AssetObserver<Texture2D> texture_observer(material_core.TextureIDs[i]);
						emitter << YAML::Key << "UUID" << YAML::Value << texture_observer.GetUUID();
						emitter << YAML::Key << "Filepath" << YAML::Value << texture_observer.GetSourceFilepath()->Filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
						auto& spec = texture_observer.GetCoreComponent().Specification;
						emitter << YAML::Key << "Specification" << YAML::Value << YAML::BeginMap;
							emitter << YAML::Key << "Usage" << YAML::Value << spec.Usage.ToConstCharPtr();
							emitter << YAML::Key << "Components" << YAML::Value << spec.Components.ToConstCharPtr();
							emitter << YAML::Key << "Format" << YAML::Value << spec.Format.ToConstCharPtr();
							emitter << YAML::Key << "Width" << YAML::Value << spec.Width;
							emitter << YAML::Key << "Height" << YAML::Value << spec.Height;
						emitter << YAML::EndMap;
					}
					else
					{
						emitter << YAML::Key << "UUID" << YAML::Value << 0;
						emitter << YAML::Key << "Filepath" << YAML::Value << "";
					}
					emitter << YAML::EndMap;
				}
				emitter << YAML::EndSeq;
			}
			emitter << YAML::EndMap;
			emitter << YAML::EndMap;

			sp.Clear();
		}
		emitter << YAML::EndSeq;

		emitter << YAML::EndMap;

		auto& path = assetObserver.GetFilepath();
		std::ofstream fout(Project::GetInstance()->AssetsPath / path);
		fout << emitter.c_str();
	}

	bool Model::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		auto& filepath = ECS_handle.get<ACFilepath>().Filepath;

		YAML::Node node = YAML::LoadFile(filepath.string());
		auto& core = ECS_handle.get<ACModelCore>();

		auto uuid_node = node["UUID"];
		if (uuid_node) // Base Assets don't have UUID in their file
		{
			if (ECS_handle.get<ACUUID>().UUID != node["UUID"].as<UUID>())
			{
				FE_CORE_ASSERT(false, "Not machting UUID in asset and its metafile!");
				return false;
			}
		}
		else
		{
			FE_LOG_CORE_WARN("Missing UUID in Model file");
		}

		for (const auto& render_mesh_node : node)
		{
			auto render_mesh_ID = AssetManager::GetOrCreateAssetWithUUID(render_mesh_node.as<UUID>());
			core.RenderMeshIDs.emplace_back(render_mesh_ID);
		}

		return true;
	}
}