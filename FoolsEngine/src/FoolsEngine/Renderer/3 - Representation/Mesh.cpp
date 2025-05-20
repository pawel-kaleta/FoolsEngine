#include "FE_pch.h"
#include "Mesh.h"

#include "FoolsEngine\Renderer\1 - Primitives\VertexData.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Assets\Loaders\GeometryLoader.h"

namespace fe
{
	void ACMeshData::Init()
	{
		Specification.Init();
		if (Data) GeometryLoader::UnloadMesh(Data);
		Data = nullptr;
	}

	ACMeshData::~ACMeshData() { if (Data) GeometryLoader::UnloadMesh(Data); }

	void MeshUser::SendDataToGPU(GDIType GDI) const
	{
		if (AllOf<ACGPUBuffers>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return;
		}

		auto& ACData = Get<ACMeshData>();
		auto& spec = ACData.Specification;
		auto& buffersComp = Emplace<ACGPUBuffers>();

		FE_CORE_ASSERT(ACData.Data, "Missing mesh data");

		buffersComp.VertexBuffer = VertexBuffer::Create(ACData.GetVertexArrayPtr(), (spec.VertexCount * sizeof(Vertex)));
		buffersComp.VertexBuffer->SetLayout(Vertex::GetLayout());

		buffersComp.IndexBuffer = IndexBuffer::Create(ACData.GetIndexArrayPtr(), spec.IndexCount);
		buffersComp.IndexBuffer->Bind();

		buffersComp.VertexBuffer->SetIndexBuffer(buffersComp.IndexBuffer);
	}

	void MeshUser::UnloadFromCPU() const
	{
		auto& data = Get<ACMeshData>().Data;
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

		auto shading_model_observer = materialObserver.GetShadingModel().Observe();
		auto shader_user = AssetHandle<Shader>(shading_model_observer.GetShaderID()).Use();

		auto GDI = Renderer::GetActiveGDItype();

		for (const auto& uniform : shading_model_observer.GetUniforms())
		{
			shader_user.UploadUniform(
				GDI,
				uniform,
				(void*)materialObserver.GetUniformValuePtr(uniform)
			);
		}

		auto& textureSlots = shading_model_observer.GetTextureSlots();
		RenderTextureSlotID rendererTextureSlot = 0;
		
		for (auto& textureSlot : textureSlots)
		{
			auto& texture = materialObserver.GetTexture(textureSlot).Use();

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
}