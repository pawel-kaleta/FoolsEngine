#include "FE_pch.h"
#include "Mesh.h"

#include "FoolsEngine\Renderer\1 - Primitives\VertexData.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Assets\Loaders\MeshLoader.h"

namespace fe
{
	bool Mesh::IsKnownSourceExtension(const std::filesystem::path& extension)
	{
		return MeshLoader::IsKnownExtension(extension);
	}

	void Mesh::SendDataToGPU(GDIType GDI, void* data)
	{
		if (AllOf<ACGPUBuffers>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return;
		}

		FE_CORE_ASSERT(data, "Missing mesh data");

		auto& meshData = *(MeshData*)data;

		Ref<VertexBuffer> vb = VertexBuffer::Create((float*)meshData.Vertices.data(), (uint32_t)(meshData.Vertices.size() * sizeof(Vertex)));
		vb->SetLayout({
			{ ShaderData::Type::Float3, "a_Position" },
			{ ShaderData::Type::Float3, "a_Normal" },
			{ ShaderData::Type::Float2, "a_TexCoord" }
		});

		Ref<IndexBuffer> ib = IndexBuffer::Create(meshData.Indices.data(), (uint32_t)meshData.Indices.size());
		ib->Bind();
		vb->SetIndexBuffer(ib);

		auto& buffersComp = Emplace<ACGPUBuffers>();
		buffersComp.IndexBuffer = ib;
		buffersComp.VertexBuffer = vb;
	}

	void Mesh::UnloadFromCPU()
	{
		auto& dataLocation = Get<ACDataLocation>();
		if (!dataLocation.Data) return;

		auto& meshData = *(MeshData*)dataLocation.Data;

		meshData.Indices.clear();
		meshData.Vertices.clear();
	}

	void Mesh::UnloadFromGPU()
	{
		if (!AllOf<ACGPUBuffers>()) return;

		Erase<ACGPUBuffers>();
	}

	void Mesh::Draw(AssetHandle<MaterialInstance> materialInstance)
	{
		if (AllOf<ACGPUBuffers>())
		{
			FE_CORE_ASSERT(false, "Mesh not uploaded to GPU");
			return;
		}

		auto& material_instance = materialInstance.Use();
		auto& material = material_instance.GetMaterial().Observe();
		auto& shader = material.GetShader().Use();

		auto GDI = Renderer::GetActiveGDItype();

		for (auto& uniform : material.GetUniforms())
		{
			shader.UploadUniform(
				GDI,
				uniform,
				material_instance.GetUniformValuePtr(uniform)
			);
		}

		auto& textureSlots = material.GetTextureSlots();
		RenderTextureSlotID rendererTextureSlot = 0;
		
		for (auto& textureSlot : textureSlots)
		{
			auto& texture = material_instance.GetTexture(textureSlot).Use();

			if (texture.IsValid())
			{
				texture.Bind(GDI, rendererTextureSlot);
			}
			else
			{
				AssetHandle<Texture2D>((AssetID)BaseAssets::Textures2D::Default).Use().Bind(GDI, rendererTextureSlot);
			}

			shader.BindTextureSlot(GDI, textureSlot, rendererTextureSlot);

			rendererTextureSlot++;
		}

		auto gpuBuffers = Get<ACGPUBuffers>();

		gpuBuffers.VertexBuffer->Bind();
		gpuBuffers.IndexBuffer->Bind();
		RenderCommands::DrawIndexed(gpuBuffers.VertexBuffer.get());
	}

	void MeshBuilder::Create(AssetUser<Mesh>& textureUser)
	{
		FE_CORE_ASSERT(m_Data, "Mesh data pointer not set");
		if (!m_Data) return;

		FE_CORE_ASSERT(m_Specification.IndicesCount != 0, "No indices");
		FE_CORE_ASSERT(m_Specification.VertexCount != 0, "No verticies");

		if (m_Specification.IndicesCount != 0) return;
		if (m_Specification.VertexCount != 0) return;

		textureUser.GetDataLocation().Data = m_Data;
		textureUser.GetOrEmplaceSpecification() = m_Specification;
	}
}