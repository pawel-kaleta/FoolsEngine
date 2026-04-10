#include "FE_pch.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/3 - Command/ProgramState.h"

namespace fe::Command
{
	void ProgramState_OpenGL::BindTextureSamplerToRendererTextureSlot(const Resource::RProgram& program, UInt samplerIndex, RenderTextureSlotID renderTextureSlot)
	{
		const Resource::RProgram_OpenGL* opengl_program = (const Resource::RProgram_OpenGL*) & program;
		GLint sampler_location = opengl_program->BindingLocations.TextureSamplers[samplerIndex];
		GLuint texture_unit = renderTextureSlot;
		glUniform1ui(sampler_location, texture_unit);
	}

	//void BindTextureSamplerToRendererTextureSlot(Resource::RProgram_OpenGL& program, String samplerName, RenderTextureSlotID rendererTextureSlot)
	//{
	//	const auto& lib = Description::Library::Get();
	//	const auto& spec = lib.ProgramSpecs[program.SpecificationID];
	//
	//	for (size_t i = 0; i < spec.TextureSamplerIDs.Count; ++i)
	//	{
	//		const auto& sampler = lib.TextureSamplers[spec.TextureSamplerIDs[i]];
	//
	//		if (CompareStringsEqual(sampler.Name, samplerName))
	//		{
	//			GLint samplerLocation = program.BindingLocations.TextureSamplers[i];
	//			BindTextureSamplerToRendererTextureSlot(samplerLocation, rendererTextureSlot);
	//			return;
	//		}
	//	}
	//}

	void ProgramState_OpenGL::UpladUniform(const Resource::RProgram& program, UInt uniformIndex, Splice<Byte> data)
	{
		const Resource::RProgram_OpenGL* opengl_program = (const Resource::RProgram_OpenGL*) & program;
		const auto& spec = Description::Library::Get().ProgramSpecs[program.SpecificationID];
		const auto& uniforms = Description::Library::Get().BufferLayouts[spec.MainUniformsLayoutID];
		const auto& uniform = uniforms.Elements[uniformIndex];
	
		Description::Data::Type type = uniform.Type;
		const auto& count = uniform.Count;
		GLint location = opengl_program->BindingLocations.MainUniforms[uniformIndex];
	
		FE_CORE_ASSERT(data.Count == uniform.Size() * count, "Wrong data size for this uniform type");
	
		if (data.Count < uniform.Size() * count || !count)
			return;

		switch (type)
		{
		case Description::Data::Type::None:
			FE_CORE_ASSERT(false, "Unknown Shader Data Type of uniform!");
			return;
		case Description::Data::Type::Bool:
		case Description::Data::Type::Int:
			glUniform1iv(location, count, (GLint*)data.Elements);
			return;
		case Description::Data::Type::Bool2:
		case Description::Data::Type::Int2:
			glUniform2iv(location, count, (GLint*)data.Elements);
			return;
		case Description::Data::Type::Bool3:
		case Description::Data::Type::Int3:
			glUniform3iv(location, count, (GLint*)data.Elements);
			return;
		case Description::Data::Type::Bool4:
		case Description::Data::Type::Int4:
			glUniform4iv(location, count, (GLint*)data.Elements);
			return;
		case Description::Data::Type::UInt:
			glUniform1uiv(location, count, (GLuint*)data.Elements);
			return;
		case Description::Data::Type::UInt2:
			glUniform2uiv(location, count, (GLuint*)data.Elements);
			return;
		case Description::Data::Type::UInt3:
			glUniform3uiv(location, count, (GLuint*)data.Elements);
			return;
		case Description::Data::Type::UInt4:
			glUniform4uiv(location, count, (GLuint*)data.Elements);
			return;
		case Description::Data::Type::Float:
			glUniform1fv(location, count, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Float2:
			glUniform2fv(location, count, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Float3:
			glUniform3fv(location, count, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Float4:
			glUniform4fv(location, count, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Double:
		case Description::Data::Type::Double2:
		case Description::Data::Type::Double3:
		case Description::Data::Type::Double4:
			FE_CORE_ASSERT(false, "Double precision Shader Data Type of uniform not supported!");
			return;
		case Description::Data::Type::Mat2:
			glUniformMatrix2fv(location, count, false, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Mat2x3:
			glUniformMatrix2x3fv(location, count, false, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Mat2x4:
			glUniformMatrix2x4fv(location, count, false, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Mat3x2:
			glUniformMatrix3x2fv(location, count, false, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Mat3:
			glUniformMatrix3fv(location, count, false, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Mat3x4:
			glUniformMatrix3x4fv(location, count, false, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Mat4x2:
			glUniformMatrix4x2fv(location, count, false, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Mat4x3:
			glUniformMatrix4x3fv(location, count, false, (GLfloat*)data.Elements);
			return;
		case Description::Data::Type::Mat4:
			glUniformMatrix4fv(location, count, false, (GLfloat*)data.Elements);
			return;
		default:
			FE_CORE_ASSERT(false, "Unrecognised Shader Data Type of uniform!");
			return;
		}
	}
	
	//void UploadUniform(Resource::RProgram_OpenGL& program, String uniformName, Splice<Byte> data)
	//{
	//	const auto& spec = Description::Library::Get().ProgramSpecs[program.SpecificationID];
	//
	//	const auto& uniforms = Description::Library::Get().BufferLayouts[spec.MainUniformsLayoutID];
	//
	//	for (size_t i = 0; i < uniforms.Elements.Count; ++i)
	//	{
	//		const auto& uniform = uniforms.Elements[i];
	//
	//		if (CompareStringsEqual(uniform.Name, uniformName))
	//		{
	//			Description::Data::Type type = uniform.Type;
	//			GLsizei count = uniform.Count;
	//			GLint location = program.BindingLocations.MainUniforms[i];
	//
	//			FE_CORE_ASSERT(data.Count == Description::Data::SizeOfType(type), "Wrong data size for this uniform type");
	//
	//			if (data.Count >= Description::Data::SizeOfType(type))
	//				UploadUniform(type, count, location, data);
	//
	//			return;
	//		}
	//	}
	//}

}