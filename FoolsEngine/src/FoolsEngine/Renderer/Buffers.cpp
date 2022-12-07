#include "FE_pch.h"

#include "Buffers.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLBuffers.h"
#include "Renderer.h"

namespace fe
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case Renderer::RendererAPI::none:
			FE_CORE_ASSERT(false, "Renderer::RendererAPI::none currently not supported!");
			return nullptr;
		case Renderer::RendererAPI::OpenGL:
			return (VertexBuffer*) new OpenGLVertexBuffer(vertices, size);
		}

		FE_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}
	
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case Renderer::RendererAPI::none:
			FE_CORE_ASSERT(false, "Renderer::RendererAPI::none currently not supported!");
			return nullptr;
		case Renderer::RendererAPI::OpenGL:
			return (IndexBuffer*) new OpenGLIndexBuffer(indices, count);
		}

		FE_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}
}