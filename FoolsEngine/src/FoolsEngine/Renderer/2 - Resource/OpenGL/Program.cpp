#include "FE_pch.h"

#include "FoolsEngine/Foundation/Memory/Scratchpad.h"
#include "FoolsEngine/Foundation/Memory/Pile.h"

#include "FoolsEngine/Assets/AssetAccessors.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/Program.h"
#include "FoolsEngine/Renderer/4 - Representation/Shader.h"

namespace fe::Resource
{
	void Program_OpenGL::Create()
	{
		FE_PROFILER_FUNC();

		Scratchpad sp;

		GLuint ProgramOpenGLID = glCreateProgram();

		std::pmr::vector<GLuint> shaders_OpenGL(&sp);

		for (UInt i =0; i<Shaders.Count; i++)
		{
			AssetObserver<Shader> shader_observer(Shaders[i]);
			shaders_OpenGL.push_back(shader_observer.GetResourceComponent<GAPIType::OpenGL>().Shader.ShaderOpenGLID);
		}

		for (int i = 0; i < shaders_OpenGL.size(); i++)
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

			std::pmr::vector<GLchar> info_log(log_length, &sp);
			glGetProgramInfoLog(ProgramOpenGLID, log_length, &log_length, info_log.data());

			glDeleteProgram(ProgramOpenGLID);
			ProgramOpenGLID = -1;

			FE_LOG_CORE_ERROR("{0}", info_log.data());
			FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
			return;
		}

		for (int i = 0; i < shaders_OpenGL.size(); i++)
		{
			glDetachShader(ProgramOpenGLID, shaders_OpenGL[i]); // do we need this?
		}

		const auto& library = Description::Library::Get();

		const auto& spec = library.ProgramSpecs[(UInt)SpecificationID];

		const auto& uniforms = library.BufferLayouts[spec.MainUniformsLayoutID];

		Pile p;
		for (UInt i = 0; i < uniforms.Elements.Count; i++)
		{
			const auto c_name = uniforms.Elements[i].Name.GetCString(&p);
			GLint location = glGetUniformLocation(ProgramOpenGLID, c_name.Data);

			//static_assert(false, "allocate splice");
			BindingLocations.MainUniforms[i] = location; // allocate first!
			p.Clear();
		}

		for (UInt i = 0; i < spec.TextureSamplerIDs.Count; i++)
		{
			const auto& texture_sampler_id = spec.TextureSamplerIDs[i];
			const auto& texture_sampler = library.TextureSamplers[texture_sampler_id];
			const auto c_name = texture_sampler.Name.GetCString(&p);
			GLint location = glGetUniformLocation(ProgramOpenGLID, c_name.Data);

			//static_assert(false, "allocate splice");
			BindingLocations.TextureSamplers[i] = location;
		}
	}

	void Program_OpenGL::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteProgram(ProgramOpenGLID);

		//Deallocate splices
	}
}