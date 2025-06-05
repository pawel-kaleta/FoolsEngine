#include "FE_pch.h"
#include "Mesh.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Assets\Loaders\GeometryLoader.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

namespace fe
{
	void ACMeshCore::Init()
	{
		Specification.Init();
		if (Data) GeometryLoader::UnloadMesh(Data);
		Data = nullptr;
	}

	ACMeshCore::~ACMeshCore() { if (Data) GeometryLoader::UnloadMesh(Data); }

	void MeshUser::SendDataToGPU(GDIType GDI) const
	{
		if (AllOf<ACGPUBuffers>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return;
		}

		auto& ACData = Get<ACMeshCore>();
		auto& spec = ACData.Specification;
		auto& buffersComp = Emplace<ACGPUBuffers>();

		FE_CORE_ASSERT(ACData.Data, "Missing mesh data");

		buffersComp.VertexBuffer = VertexBuffer::Create(ACData.GetVertexArrayPtr(), (spec.VertexCount * sizeof(VertexData::Vertex)));
		buffersComp.VertexBuffer->SetLayout(VertexData::Vertex::GetLayout());

		buffersComp.IndexBuffer = IndexBuffer::Create(ACData.GetIndexArrayPtr(), spec.IndexCount);
		buffersComp.IndexBuffer->Bind();

		buffersComp.VertexBuffer->SetIndexBuffer(buffersComp.IndexBuffer);
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
			FE_CORE_ASSERT(false, "Mesh not uploaded to GPU");
			return;
		}

		auto& material_core = materialObserver.GetCoreComponent();
		auto shading_model_observer = AssetObserver<ShadingModel>(material_core.ShadingModelID);
		auto& sm_core = shading_model_observer.GetCoreComponent();
		auto shader_user = AssetUser<Shader>(sm_core.ShaderID);

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
			auto texture = materialObserver.GetTexture(material_core, textureSlot).Use();

			if (texture.IsValid())
			{
				texture.Bind(GDI, rendererTextureSlot);
			}
			else
			{
				FE_CORE_ASSERT(false, "Not implemented default texture");		
				Renderer::BaseAssets.Textures.Default.Use().Bind(GDI, rendererTextureSlot);
			}

			shader_user.BindTextureSlot(GDI, textureSlot, rendererTextureSlot);

			rendererTextureSlot++;
		}

		auto gpuBuffers = Get<ACGPUBuffers>();

		gpuBuffers.VertexBuffer->Bind();
		RenderCommands::DrawIndexed(gpuBuffers.VertexBuffer.get());
	}

	void Mesh::Serialize(const AssetObserver<Mesh>& assetObserver)
	{
		auto& core = assetObserver.GetCoreComponent();

		YAML::Emitter emitter;

		emitter << YAML::Key << "Source Filepath" << YAML::Value << assetObserver.GetSourceFilepath()->Filepath.string();
		emitter << YAML::Key << "Vartex Count" << YAML::Value << core.Specification.VertexCount;
		emitter << YAML::Key << "Index Count" << YAML::Value << core.Specification.IndexCount;

		std::ofstream fout(assetObserver.GetFilepath());
		fout << emitter.c_str();
	}

	bool Mesh::Deserialize(AssetID assetID)
	{
		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		const auto& filepath = ECS_handle.get<ACFilepath>().Filepath;
		YAML::Node node = YAML::LoadFile(filepath.string());

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