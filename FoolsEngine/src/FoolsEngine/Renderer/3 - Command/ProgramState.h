#pragma once

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/2 - Resource/RProgram.h"


namespace fe::Command
{
	namespace ProgramState
	{
		template <GAPIType::ValueType GAPI> void BindTextureSamplerToRendererTextureSlot(const Resource::RProgram<GAPI>& program, UInt samplerIndex, RenderTextureSlotID renderTextureSlot);
		template <GAPIType::ValueType GAPI> void UpladUniform(const Resource::RProgram<GAPI>& program, UInt uniformIndex, Splice<Byte> data);

		//Vulkan
		template <> void BindTextureSamplerToRendererTextureSlot<GAPIType::Vulkan>(const Resource::RProgram<GAPIType::Vulkan>& program, UInt samplerIndex, RenderTextureSlotID rendererTextureSlot);

		//OpenGL
		template <> void BindTextureSamplerToRendererTextureSlot<GAPIType::OpenGL>(	const Resource::RProgram<GAPIType::OpenGL>	& program, UInt samplerIndex, RenderTextureSlotID renderTextureSlot);
		inline		void BindTextureSamplerToRendererTextureSlot_OpenGL(			const Resource::RProgram_OpenGL				& program, UInt samplerIndex, RenderTextureSlotID renderTextureSlot) { BindTextureSamplerToRendererTextureSlot<GAPIType::OpenGL>(program, samplerIndex, renderTextureSlot); };

		template <> void UpladUniform<GAPIType::OpenGL>(const Resource::RProgram<GAPIType::OpenGL>	& program, UInt uniformIndex, Splice<Byte> data);
		inline		void UpladUniform_OpenGL(			const Resource::RProgram_OpenGL				& program, UInt uniformIndex, Splice<Byte> data) { UpladUniform<GAPIType::OpenGL>(program, uniformIndex, data); }

		//void BindTextureSamplerToRendererTextureSlot(Resource::RProgram_OpenGL& program, String samplerName, RenderTextureSlotID rendererTextureSlot);
		//void UploadUniform(Description::Data::Type type, GLsizei count, GLint location, Splice<Byte> data);
		//void UploadUniform(Resource::RProgram_OpenGL& program, UInt uniformIndex, Splice<Byte> data);
		//void UploadUniform(Resource::RProgram_OpenGL& program, String uniformName, Splice<Byte> data);
	}
}