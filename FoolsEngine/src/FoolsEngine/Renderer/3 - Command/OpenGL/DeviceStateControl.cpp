#include "FE_pch.h"

#include "FoolsEngine\Renderer\3 - Command\DeviceStateControl.h"

namespace fe::Command
{
	namespace DeviceStateControl::OpenGL
	{
		void BindVertexArray(const Resource::VertexArray_OpenGL& vertexBinding)
		{
			glBindVertexArray(vertexBinding.OpenGLID);
		}

		void BindTextureToRendererTextureSlot(uint32_t rendererTextureSlot, const Resource::Texture_OpenGL& texture)
		{
			glBindTextureUnit(rendererTextureSlot, texture.TextureOpenGLID);
		}
	}
}