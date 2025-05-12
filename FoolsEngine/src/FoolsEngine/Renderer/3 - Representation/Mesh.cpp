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
	void Mesh::SendDataToGPU(GDIType GDI, void* data)
	{
		if (AllOf<ACGPUBuffers>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return;
		}

		FE_CORE_ASSERT(data, "Missing mesh data");

		MeshData& meshData = *(MeshData*)GetDataLocation().Data;
		auto& spec = GetSpecification();

		auto& buffersComp = Emplace<ACGPUBuffers>();

		buffersComp.VertexBuffer = VertexBuffer::Create(meshData.GetVertexArrayPtr(spec.IndicesCount), (spec.VertexCount * sizeof(Vertex)));
		buffersComp.VertexBuffer->SetLayout(Vertex::GetLayout());

		buffersComp.IndexBuffer = IndexBuffer::Create(meshData.GetIndexArrayPtr(), spec.IndicesCount);
		buffersComp.IndexBuffer->Bind();

		buffersComp.VertexBuffer->SetIndexBuffer(buffersComp.IndexBuffer);
	}

	void Mesh::UnloadFromCPU()
	{
		auto& data = GetDataLocation().Data;
		if (data)
		{
			GeometryLoader::UnloadMesh(data);
			data = nullptr;
		}
	}

	void Mesh::PlaceCoreComponents()
	{
		Emplace<ACMeshSpecification>().Init();
	}

	void Mesh::Release()
	{
		if (!AllOf<ACGPUBuffers>()) return;

		Erase<ACGPUBuffers>();
	}

	void Mesh::Draw(const AssetObserver<MaterialInstance>& miObserver)
	{
		if (!AllOf<ACGPUBuffers>())
		{
			FE_CORE_ASSERT(false, "Mesh not uploaded to GPU");
			return;
		}

		//auto& material_instance = materialInstance.Use();
		auto& material = miObserver.GetMaterial().Observe();
		auto& shader = AssetHandle<Shader>(material.GetShaderID()).Use();

		auto GDI = Renderer::GetActiveGDItype();

		for (auto& uniform : material.GetUniforms())
		{
			shader.UploadUniform(
				GDI,
				uniform,
				(void*)miObserver.GetUniformValuePtr(uniform)
			);
		}

		auto& textureSlots = material.GetTextureSlots();
		RenderTextureSlotID rendererTextureSlot = 0;
		
		for (auto& textureSlot : textureSlots)
		{
			auto& texture = miObserver.GetTexture(textureSlot).Use();

			if (texture.IsValid())
			{
				texture.Bind(GDI, rendererTextureSlot);
			}
			else
			{
				FE_CORE_ASSERT(false, "Not implemented default texture");
				//AssetHandle<Texture2D>((AssetID)BaseAssets::Textures2D::Default).Use().Bind(GDI, rendererTextureSlot);
			}

			shader.BindTextureSlot(GDI, textureSlot, rendererTextureSlot);

			rendererTextureSlot++;
		}

		auto gpuBuffers = Get<ACGPUBuffers>();

		gpuBuffers.VertexBuffer->Bind();
		//gpuBuffers.IndexBuffer->Bind();
		RenderCommands::DrawIndexed(gpuBuffers.VertexBuffer.get());
	}
}