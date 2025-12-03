#include "FE_pch.h"

#include "FoolsEngine\Renderer\3 - Command\ResourceStateControl.h"

namespace fe::Command
{
	namespace ResourceStateControl::OpenGL
	{
		void BindToRendererTextureSlot(uint32_t rendererTextureSlot, const Resource::Texture_OpenGL& texture)
		{
			glBindTextureUnit(rendererTextureSlot, texture.m_OpenGLID);
		}

		void BindVertexBuffer(const Resource::VertexBuffer_OpenGL& vertexBuffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.m_VertexBufferID);
		}

		void BindVertexBinding(const Resource::VertexBinding_OpenGL& vertexBinding)
		{
			glBindVertexArray(vertexBinding.m_VertexArrayID);
		}

		void BindIndexBuffer(const Resource::IndexBuffer_OpenGL& indexBuffer)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.m_IndexBufferID);
		}

		
	}
}