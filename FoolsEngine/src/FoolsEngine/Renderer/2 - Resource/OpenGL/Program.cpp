#include "FE_pch.h"

#include "FoolsEngine/Foundation/Memory/Pile.h"

#include "FoolsEngine/Assets/AssetAccessors.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/Program.h"
#include "FoolsEngine/Renderer/4 - Representation/Shader.h"

namespace fe::Resource
{
	void Program_OpenGL::Create()
	{
		FE_PROFILER_FUNC();

		GLuint ProgramOpenGLID = glCreateProgram();

		Pile p;
		auto shaders_OpenGL = p.Allocate<GLuint>(Shaders.Count);

		for (UInt i =0; i<Shaders.Count; i++)
		{
			if (Shaders[i] == NullAssetID) continue; // should not be possible
			AssetObserver<Shader> shader_observer(Shaders[i]);
			shaders_OpenGL[i] = shader_observer.GetResourceOpenGL()->Shader.ShaderOpenGLID;
		}

		for (int i = 0; i < shaders_OpenGL.Count; i++)
			glAttachShader(ProgramOpenGLID, shaders_OpenGL[i]);

		GLint linking_success = 0;
		{
			FE_PROFILER_SCOPE("OpenGL Shader linking");
			glLinkProgram(ProgramOpenGLID);
			glGetProgramiv(ProgramOpenGLID, GL_LINK_STATUS, (int*)&linking_success);
		}

		if (linking_success == GL_FALSE)
		{
			GLint log_length = 0;
			glGetProgramiv(ProgramOpenGLID, GL_INFO_LOG_LENGTH, &log_length);

			auto info_log = p.Allocate<GLchar>(log_length);
			glGetProgramInfoLog(ProgramOpenGLID, log_length, &log_length, info_log.Elements);

			glDeleteProgram(ProgramOpenGLID);
			ProgramOpenGLID = -1;

			FE_LOG_CORE_ERROR("{0}", info_log.Elements); // is this null terminated?
			FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
			return;
		}

		for (int i = 0; i < shaders_OpenGL.Count; i++)
		{
			glDetachShader(ProgramOpenGLID, shaders_OpenGL[i]); // do we need this?
		}


		const auto& library = Description::Library::Get();

		const auto& spec = library.ProgramSpecs[(UInt)SpecificationID];

		const auto& uniforms = library.BufferLayouts[spec.MainUniformsLayoutID];

		BindingLocations.MainUniforms = Context::Allocators::Default->Allocate<GLint>(uniforms.Elements.Count);
		for (UInt i = 0; i < uniforms.Elements.Count; i++)
		{
			const auto c_name = uniforms.Elements[i].Name.GetCString(&p);
			GLint location = glGetUniformLocation(ProgramOpenGLID, c_name.Data);

			BindingLocations.MainUniforms[i] = location; // allocate first!
			p.Clear();
		}

		BindingLocations.TextureSamplers = Context::Allocators::Default->Allocate<GLint>(spec.TextureSamplerIDs.Count);
		for (UInt i = 0; i < spec.TextureSamplerIDs.Count; i++)
		{
			const auto& texture_sampler_id = spec.TextureSamplerIDs[i];
			const auto& texture_sampler = library.TextureSamplers[texture_sampler_id];
			const auto c_name = texture_sampler.Name.GetCString(&p);
			GLint location = glGetUniformLocation(ProgramOpenGLID, c_name.Data);

			BindingLocations.TextureSamplers[i] = location;
			p.Clear();
		}
	}

	void Program_OpenGL::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteProgram(ProgramOpenGLID);

		Context::Allocators::Default->Deallocate(BindingLocations.MainUniforms);
		Context::Allocators::Default->Deallocate(BindingLocations.TextureSamplers);
	}
}