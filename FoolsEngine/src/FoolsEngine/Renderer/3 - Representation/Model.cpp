#include "FE_pch.h"
#include "Model.h"

namespace fe
{
	Ref<Material> Mesh::Default3DMaterial = Ref<Material>();

	// swaps vectors' data with internal vectors
	Mesh::Mesh(Ref<MaterialInstance> materialInstance, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
	{
		m_MaterialInstance = materialInstance;

		m_Vertices.swap(vertices);
		m_Indices.swap(indices);
		//m_Textures.swap(textures);

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
		//m_VertexBuffer->Bind();
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
			FE_CORE_ASSERT(false, "Attempt to free mesh buffers from CPU, when they are not stored on CPU");
			break;
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

	void Mesh::Draw(Transform& transform, glm::mat4 VP, EntityID entityID)
	{
		if (m_DataLocation != DataLocation::GPU && m_DataLocation != DataLocation::CPU_GPU)
		{
			FE_CORE_ASSERT(false, "Mesh not uploaded to GPU");
			return;
		}

		uint32_t rendererTextureSlot = 0;
		auto whiteTexture = TextureLibrary::Get("Base2DTexture");

		auto& shader = m_MaterialInstance->GetMaterial()->GetShader();
		auto& textureSlots = m_MaterialInstance->GetMaterial()->GetTextureSlots();
		shader->Bind();
		
		shader->UploadUniform(
			Uniform("u_ViewProjection", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(VP)
		);
		shader->UploadUniform(
			Uniform("u_EntityID", ShaderData::Type::UInt),
			(void*)&entityID
		);

		for (unsigned int i = 0; i < textureSlots.size(); i++)
		{
			if (i < m_Textures.size())
			{
				if (m_Textures[i])
				{
					m_Textures[i]->Bind(rendererTextureSlot);
				}
				else
				{
					whiteTexture->Bind(rendererTextureSlot);
				}
			}
			else
			{
				whiteTexture->Bind(rendererTextureSlot);
			}

			
			shader->BindTextureSlot(textureSlots[i], rendererTextureSlot);

			rendererTextureSlot++;
		}

		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();
		RenderCommands::DrawIndexed(m_VertexBuffer.get());
	}
}