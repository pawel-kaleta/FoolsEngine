#include "FE_pch.h"
#include "Mesh.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Assets\Loaders\GeometryLoader.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

#include "FoolsEngine\Core\Project.h"

namespace fe
{
	void ACMeshCore::Init()
	{
		Specification.Init();
		if (Data) GeometryLoader::UnloadMesh(Data);
		Data = nullptr;
	}

	ACMeshCore::~ACMeshCore() { if (Data) GeometryLoader::UnloadMesh(Data); }

	bool MeshUser::SendDataToGPU(GDIType GDI) const
	{
		if (AllOf<ACGPUBuffers>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return false;
		}

		auto& ACData = Get<ACMeshCore>();
		auto& spec = ACData.Specification;
		auto& buffersComp = Emplace<ACGPUBuffers>();

		if (!ACData.Data)
			return false;

		buffersComp.VertexBuffer = VertexBuffer::Create(ACData.GetVertexArrayPtr(), (spec.VertexCount * sizeof(VertexData::Vertex)));
		buffersComp.VertexBuffer->SetLayout(VertexData::Vertex::GetLayout());

		buffersComp.IndexBuffer = IndexBuffer::Create(ACData.GetIndexArrayPtr(), spec.IndexCount);

		buffersComp.VertexBuffer->SetIndexBuffer(buffersComp.IndexBuffer);

		return true;
	}

	void MeshUser::UnloadFromCPU() const
	{
		auto& data = Get<ACMeshCore>().Data;
		if (data)
		{
			GeometryLoader::UnloadMesh(data);
			data = nullptr;
		}
	}

	void MeshUser::Release() const
	{
		if (!AllOf<ACGPUBuffers>()) return;

		Erase<ACGPUBuffers>();
	}

	// mesh should not draw itself
	void MeshObserver::Draw(const AssetObserver<Material>& materialObserver) const
	{
		if (!AllOf<ACGPUBuffers>())
		{
			//FE_CORE_ASSERT(false, "Mesh not uploaded to GPU");
			return;
		}

		auto& material_core = materialObserver.GetCoreComponent();
		AssetObserver<ShadingModel> shading_model_observer(material_core.ShadingModelID);
		auto& sm_core = shading_model_observer.GetCoreComponent();
		AssetUser<Shader> shader_user(sm_core.ShaderID);

		auto GDI = Renderer::GetActiveGDItype();

		char* uniform_data_ptr = (char*)material_core.UniformsData;

		for (const auto& uniform : sm_core.Uniforms)
		{
			shader_user.UploadUniform(
				GDI,
				uniform,
				(void*)uniform_data_ptr
			);

			uniform_data_ptr += uniform.GetCount() * uniform.GetSize();
		}

		RenderTextureSlotID rendererTextureSlot = 0;
		for (size_t i = 0; i < sm_core.TextureSlots.size(); ++i)
		{
			auto textureID = material_core.TextureIDs[i];
			auto& texture_slot = sm_core.TextureSlots[i];

			if (textureID != NullAssetID)
			{
				AssetUser<Texture2D> texture(textureID);
				texture.Bind(GDI, rendererTextureSlot);
			}
			else
			{
				Renderer::BaseAssets.Textures.Default.Use().Bind(GDI, rendererTextureSlot);
			}

			shader_user.BindTextureSlot(GDI, texture_slot, rendererTextureSlot);

			rendererTextureSlot++;
		}

		auto gpuBuffers = Get<ACGPUBuffers>();

		gpuBuffers.VertexBuffer->Bind();
		RenderCommands::DrawIndexed(gpuBuffers.VertexBuffer.get());
	}

	void Mesh::SaveMetadata(YAML::Emitter& emitter, AssetID assetID)
	{
		auto assetObserver = AssetObserver<Mesh>(assetID);
		auto& core = assetObserver.GetCoreComponent();

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << assetObserver.GetUUID();
		if (!assetObserver.AllOf<ACMasterAsset>())
			emitter << YAML::Key << "Source Filepath" << YAML::Value << assetObserver.GetSourceFilepath()->Filepath.string();
		emitter << YAML::Key << "Vartex Count" << YAML::Value << core.Specification.VertexCount;
		emitter << YAML::Key << "Index Count" << YAML::Value << core.Specification.IndexCount;
		emitter << YAML::EndMap;
	}

	bool Mesh::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();

		const auto& filepath = reg.get<ACFilepath>(assetID).Filepath;
		auto full_filepath = Project::GetInstance()->AssetsPath / filepath;

		YAML::Node node;

		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			node = YAML::LoadFile(full_filepath.string());
		}

		auto uuid_node = node["UUID"];
		if (uuid_node) // Base Assets don't have UUID in their file
		{
			if (reg.get<ACUUID>(assetID).UUID != node["UUID"].as<UUID>())
			{
				FE_CORE_ASSERT(false, "Not machting UUID in asset and its serialized node");
				return false;
			}
		}
		else
		{
			FE_LOG_CORE_WARN("Missing UUID in Mesh serialized node");
		}

		const auto& source_filepath_node = node["Source Filepath"];
		if (!source_filepath_node) return false;
		AssetManager::SetSourcePath(assetID, source_filepath_node.as<std::string>());

		const auto& vertex_count_node = node["Vartex Count"];
		const auto& index_count_node = node["Index Count"];

		if (!vertex_count_node ||
			!index_count_node)
			return false;

		auto& core = reg.get<ACMeshCore>(assetID);
		core.Specification.VertexCount = vertex_count_node.as<uint32_t>();
		core.Specification.IndexCount = index_count_node.as<uint32_t>();

		return true;
	}

	AssetID Mesh::LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();

		const auto& uuid_node = node["UUID"];
		if (!uuid_node)
		{
			FE_LOG_CORE_WARN("Missing UUID in Mesh serialized node!");
			return NullAssetID;
		}

		auto uuid = uuid_node.as<UUID>();
		if (uuid == UUID(0))
		{
			FE_LOG_CORE_WARN("Missing Mesh definition!");
			return NullAssetID;
		}

		auto asset_id = AssetManager::GetOrCreateAssetWithUUID(uuid);
		if (reg.all_of<ACRefsCounters>(asset_id)) return asset_id; // is ProjectAsset ?

		reg.emplace<ACAssetType>(asset_id).Type = AssetType::Mesh;
		reg.emplace<ACMasterAsset>(asset_id).Master = master;
		auto& mesh_core = reg.emplace<Mesh::Core>(asset_id);
		mesh_core.Init();

		const auto& vertex_count_node = node["Vartex Count"];
		const auto& index_count_node = node["Index Count"];

		if (!vertex_count_node ||
			!index_count_node)
		{
			FE_LOG_CORE_WARN("Ill defined Mesh!");
			return NullAssetID;
		}

		mesh_core.Specification.VertexCount = vertex_count_node.as<uint32_t>();
		mesh_core.Specification.IndexCount = index_count_node.as<uint32_t>();

		return asset_id;
	}
}