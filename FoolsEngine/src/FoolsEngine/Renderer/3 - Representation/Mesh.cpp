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

		for (const auto& uniform : sm_core.Uniforms)
		{
			shader_user.UploadUniform(
				GDI,
				uniform,
				(void*)materialObserver.GetUniformValuePtr(material_core, uniform)
			);
		}

		RenderTextureSlotID rendererTextureSlot = 0;
		
		for (auto& textureSlot : sm_core.TextureSlots)
		{
			auto textureID = materialObserver.GetTextureID(material_core, textureSlot);

			if (textureID != NullAssetID)
			{
				AssetUser<Texture2D> texture(textureID);
				texture.Bind(GDI, rendererTextureSlot);
			}
			else
			{
				//FE_CORE_ASSERT(false, "Not implemented default texture");		
				Renderer::BaseAssets.Textures.Default.Use().Bind(GDI, rendererTextureSlot);
			}

			shader_user.BindTextureSlot(GDI, textureSlot, rendererTextureSlot);

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
		emitter << YAML::Key << "Source Filepath" << YAML::Value << assetObserver.GetSourceFilepath()->Filepath.string();
		emitter << YAML::Key << "Vartex Count" << YAML::Value << core.Specification.VertexCount;
		emitter << YAML::Key << "Index Count" << YAML::Value << core.Specification.IndexCount;
		emitter << YAML::EndMap;

		//auto x = Project::GetInstance()->AssetsPath / assetObserver.GetFilepath();
		//std::ofstream fout(x);
		//fout << emitter.c_str();
	}

	bool Mesh::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		const auto& filepath = ECS_handle.get<ACFilepath>().Filepath;
		YAML::Node node = YAML::LoadFile((Project::GetInstance()->AssetsPath / filepath).string());

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
			FE_LOG_CORE_WARN("Missing UUID in Mesh file");
		}

		const auto& source_filepath_node = node["Source Filepath"];
		const auto& vertex_count_node = node["Vartex Count"];
		const auto& index_count_node = node["Index Count"];

		if (!source_filepath_node) return false;
		if (!vertex_count_node) return false;
		if (!index_count_node) return false;

		auto& core = ECS_handle.get<ACMeshCore>();
		core.Specification.VertexCount = vertex_count_node.as<uint32_t>();
		core.Specification.IndexCount  =  index_count_node.as<uint32_t>();

		AssetManager::SetSourcePath(assetID, source_filepath_node.as<std::string>());

		return true;
	}
}