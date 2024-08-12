#include "FE_pch.h"
#include "Mesh.h"

#include "FoolsEngine\Renderer\1 - Primitives\VertexData.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

namespace fe
{
	size_t Mesh::GetVertexCount() const
	{
		if (m_DataLocation == DataLocation::CPU || m_DataLocation == DataLocation::CPU_GPU)
		{
			return m_Vertices.size();
		}
		if (m_DataLocation == DataLocation::GPU)
		{
			return m_VertexBuffer->GetSize();
		}
		return 0;
	}

	// swaps vectors' data with internal vectors
	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, AssetHandle<MaterialInstance> materialInstance)
	{
		m_MaterialInstance = materialInstance;

		m_Vertices.swap(vertices);
		m_Indices.swap(indices);

		m_DataLocation = DataLocation::CPU;
	}

	void Mesh::UploadBuffersToGPU()
	{
		if (m_DataLocation == DataLocation::GPU || m_DataLocation == DataLocation::CPU_GPU)
		{
			FE_LOG_CORE_WARN("Mesh buffers already uploaded to GPU");
			return;
		}
		FE_CORE_ASSERT(m_DataLocation == DataLocation::CPU, "Meshbuffer not present on CPU upon an attempt to load onto a GPU");

		m_VertexBuffer = VertexBuffer::Create((float*)m_Vertices.data(), (uint32_t)(m_Vertices.size() * sizeof(Vertex)));
		m_VertexBuffer->SetLayout({
			{ ShaderData::Type::Float3, "a_Position" },
			{ ShaderData::Type::Float3, "a_Normal" },
			{ ShaderData::Type::Float2, "a_TexCoord" }
		});

		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size());
		m_IndexBuffer->Bind();
		m_VertexBuffer->SetIndexBuffer(m_IndexBuffer);

		m_DataLocation = DataLocation::CPU_GPU;
	}

	void Mesh::FreeBuffersFromCPU()
	{
		switch (m_DataLocation)
		{
		case DataLocation::None:
		case DataLocation::GPU:
			FE_CORE_ASSERT(false, "Attempt to free mesh buffers from CPU, when they are not stored on CPU");
			break;
		case DataLocation::CPU:
			m_DataLocation = DataLocation::None;
			m_Vertices.clear();
			m_Indices.clear();
			break;
		case DataLocation::CPU_GPU:
			m_DataLocation = DataLocation::GPU;
			m_Vertices.clear();
			m_Indices.clear();
			break;
		}
	}

	void Mesh::FreeBuffersFromGPU()
	{
		switch (m_DataLocation)
		{
		case DataLocation::None:
			FE_CORE_ASSERT(false, "Attempt to free mesh buffers from GPU, when they are not stored on GPU");
			break;
		case DataLocation::CPU:
			FE_CORE_ASSERT(false, "Attempt to free mesh buffers from GPU, when they are not stored on GPU");
			break;
		case DataLocation::GPU:
			m_DataLocation = DataLocation::None;
			m_VertexBuffer.reset();
			m_IndexBuffer.reset();
			break;
		case DataLocation::CPU_GPU:
			m_DataLocation = DataLocation::CPU;
			m_VertexBuffer.reset();
			m_IndexBuffer.reset();
			break;
		}
	}

	void Mesh::Draw(AssetHandle<MaterialInstance> materialInstance)
	{
		if (m_DataLocation != DataLocation::GPU && m_DataLocation != DataLocation::CPU_GPU)
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
		uint32_t rendererTextureSlot = 0;
		// markmark 
		FE_CORE_ASSERT(false, "");
		//auto whiteTexture = TextureLibrary::Get("WhiteTexture");
		for (auto& textureSlot : textureSlots)
		{
			auto& texture = material_instance.GetTexture(textureSlot).Use();

			if (texture.IsValid())
				texture.Bind(GDI, rendererTextureSlot);
			else
			{
				FE_CORE_ASSERT(false, "");
				// markmark 
				// whiteTexture->Bind(rendererTextureSlot);
			}

			shader.BindTextureSlot(GDI, textureSlot, rendererTextureSlot);

			rendererTextureSlot++;
		}

		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();
		RenderCommands::DrawIndexed(m_VertexBuffer.get());
	}
}