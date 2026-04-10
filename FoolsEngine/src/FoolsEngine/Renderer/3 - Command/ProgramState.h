#pragma once

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/2 - Resource/RProgram.h"


namespace fe::Command
{
	struct ProgramState
	{
		virtual void BindTextureSamplerToRendererTextureSlot(const Resource::RProgram& program, UInt samplerIndex, RenderTextureSlotID renderTextureSlot) = 0;
		virtual void UpladUniform(const Resource::RProgram& program, UInt uniformIndex, Splice<Byte> data) = 0;
	};

	struct ProgramState_OpenGL final : public ProgramState
	{
		virtual void BindTextureSamplerToRendererTextureSlot(const Resource::RProgram& program, UInt samplerIndex, RenderTextureSlotID rendererTextureSlot) final override;
		virtual void UpladUniform(const Resource::RProgram& program, UInt uniformIndex, Splice<Byte> data) final override;
		
		//void BindTextureSamplerToRendererTextureSlot(Resource::RProgram_OpenGL& program, String samplerName, RenderTextureSlotID rendererTextureSlot);
		//void UploadUniform(Description::Data::Type type, GLsizei count, GLint location, Splice<Byte> data);
		//void UploadUniform(Resource::RProgram_OpenGL& program, UInt uniformIndex, Splice<Byte> data);
		//void UploadUniform(Resource::RProgram_OpenGL& program, String uniformName, Splice<Byte> data);
	};
}